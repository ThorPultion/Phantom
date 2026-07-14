// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAIController.h"
#include "Components/StateTreeAIComponent.h"
#include "CoreAIPerceptionComponent.h"
#include "CoreAICharacter.h"
#include "AbilitySystemComponent.h"
#include "Interfaces/Perceivable.h"
#include "AIAttributeSet.h"
#include "AIStatePriorityData.h"
#include "GASCoreTags.h"
#include "Perception/AISense_Sight.h"

ACoreAIController::ACoreAIController()
{
	CorePerceptionComponent = CreateDefaultSubobject<UCoreAIPerceptionComponent>(TEXT("CorePerceptionComponent"));

	SetPerceptionComponent(*CorePerceptionComponent);

	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
}

void ACoreAIController::BeginPlay()
{
	Super::BeginPlay();

	// The StateTreeComponent usually starts itself if configured to do so in the Editor,
	// but you can also manually initialize and start it here if needed.

	// Binding the perception delegate
	if (CorePerceptionComponent)
	{
		CorePerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACoreAIController::OnTargetPerceptionUpdated);
	}
}

void ACoreAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledCharacter = Cast<ACoreAICharacter>(InPawn);

	if (ControlledCharacter)
	{
		AbilitySystemComponent = ControlledCharacter->GetAbilitySystemComponent();
	}

	DetectionDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UAIAttributeSet::GetDetectionLevelAttribute()).AddUObject(this, &ACoreAIController::OnDetectionLevelChanged);
}

void ACoreAIController::OnUnPossess()
{
	Super::OnUnPossess();

	// Clean up when the AI dies or changes bodies
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UAIAttributeSet::GetDetectionLevelAttribute()).Remove(DetectionDelegateHandle);
	}

	ControlledCharacter = nullptr;
	AbilitySystemComponent = nullptr;
}

void ACoreAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!IsValid(Actor)) return;

	if (Actor->Implements<UPerceivable>())
	{
		// The sensed object will decide what state tag to apply
		FGameplayTag ReactionTag = IPerceivable::Execute_GetPerceptionTag(Actor, GetTeamAttitudeTowards(*Actor), Stimulus);

		if (!ReactionTag.IsValid()) return;

		// Check if they are already in the array
		FPerceivedData* ExistingData = KnownTargets.FindByKey(Actor);

		// Apply Escalation / De-escalation Rules
		if (Stimulus.WasSuccessfullySensed())
		{
			if (ExistingData)
			{
				// ESCALATION: Only update if the new tag is a higher priority.
				// (Prevents a perception pulse from overwriting Combat with Suspicious)
				int32 CurrentScore = ControlledCharacter->PriorityData->StatePriorities.FindRef(ExistingData->DesiredStateTag);
				int32 ProposedScore = ControlledCharacter->PriorityData->StatePriorities.FindRef(ReactionTag);

				if (ProposedScore > CurrentScore)
				{
					ExistingData->DesiredStateTag = ReactionTag;
				}
			}
			else
			{
				// Brand new target, add them to memory
				FPerceivedData NewData;
				NewData.TargetActor = Actor;
				NewData.DesiredStateTag = ReactionTag;
				KnownTargets.Add(NewData);
			}
		}
		else
		{
			if (ExistingData)
			{
				// DE-ESCALATION: Sight lost! Force the downgrade (Combat -> Investigate)

				// Note: If you have a Max Age set in your AI Perception config, this stimulus 
				// will fire again with WasSuccessfullySensed() = false when they expire completely.
				// You could add logic here to remove them from KnownTargets entirely if needed.

				if (!CorePerceptionComponent->HasActiveStimulus(*Actor, UAISense::GetSenseID<UAISense_Sight>()))
				{
					// We have legitimately lost sight. Now we can downgrade to Search.
					ExistingData->DesiredStateTag = ReactionTag;
				}
			}
		}
	}

	EvaluateBestTarget();
}

void ACoreAIController::EvaluateBestTarget()
{
	// Check cached pointers
	if (!IsValid(ControlledCharacter) || !ControlledCharacter->PriorityData)
	{
		CurrentTargetActor = nullptr;
		CurrentTargetTag = FGameplayTag::EmptyTag;
		return;
	}

	FGameplayTag BestTag = FGameplayTag::EmptyTag;
	AActor* BestActor = nullptr;
	int32 HighestScore = -1;
	float ClosestDistanceSq = MAX_flt; // Track distance for tie-breakers

	// Loop through targets too find highest scored tag
	for (const FPerceivedData& TargetData : KnownTargets)
	{
		if (!IsValid(TargetData.TargetActor)) continue;

		int32 Score = 0;

		if (const int32* FoundPriority = ControlledCharacter->PriorityData->StatePriorities.Find(TargetData.DesiredStateTag))
		{
			Score = *FoundPriority;
		}

		// Calculate squared distance (cheaper than actual distance because it avoids square roots)
		const float DistanceSq = FVector::DistSquared(TargetData.TargetActor->GetActorLocation(), ControlledCharacter->GetActorLocation());

		if (Score > HighestScore)
		{
			HighestScore = Score;
			BestActor = TargetData.TargetActor;
			BestTag = TargetData.DesiredStateTag;
			ClosestDistanceSq = DistanceSq;
		}
		// TIE BREAKER: Are the priorities exactly the same?
		else if (Score == HighestScore)
		{
			// Pick the one that is physically closer
			if (DistanceSq < ClosestDistanceSq)
			{
				BestActor = TargetData.TargetActor;
				BestTag = TargetData.DesiredStateTag;
				ClosestDistanceSq = DistanceSq;
			}
		}
	}

	// Update ASC loose tags using the cached ASC
	if (CurrentTargetTag != BestTag)
	{
		// 1. Cache the old tag
		const FGameplayTag OldTag = CurrentTargetTag;

		// 2. UPDATE THE LOCK FIRST to prevent re-entrancy loops!
		CurrentTargetTag = BestTag;

		if (AbilitySystemComponent)
		{
			if (OldTag.IsValid())
			{
				// 3. This might trigger callbacks that fire EvaluateBestTarget again.
				// Because we already updated CurrentTargetTag, the inner loop will safely ignore it.
				AbilitySystemComponent->RemoveLooseGameplayTag(OldTag);
			}

			if (BestTag.IsValid())
			{
				AbilitySystemComponent->AddLooseGameplayTag(BestTag);
			}
		}
	}

	SetTarget(BestActor);
}

ETeamAttitude::Type ACoreAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	// If no team interface implemented, neutral
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other);
	if (!OtherTeamAgent)
	{
		return ETeamAttitude::Neutral;
	}

	// This objects team and other objects team
	FGenericTeamId OurTeam = GetGenericTeamId();
	FGenericTeamId TheirTeam = OtherTeamAgent->GetGenericTeamId();

	// If no team, neutral
	if (OurTeam == FGenericTeamId::NoTeam || TheirTeam == FGenericTeamId::NoTeam)
	{
		return ETeamAttitude::Neutral;
	}

	if (OurTeam == TheirTeam) return ETeamAttitude::Friendly;

	// Default to hostile for anyone not on our team
	return ETeamAttitude::Hostile;
}

void ACoreAIController::OnDetectionLevelChanged(const FOnAttributeChangeData& Data)
{
	if (!AbilitySystemComponent) return;

	// If the meter is full
	if (Data.NewValue >= AbilitySystemComponent->GetNumericAttribute(UAIAttributeSet::GetMaxDetectionAttribute()))
	{
		// Escalating our currently focused target to a combat threat
		if (CurrentTargetActor)
		{
			// SAFETY CHECK: Only force the Combat tag if our eyeballs are actually on the target!
			// If we are circling and drop sight, this prevents the Attribute from fighting Perception.
			if (CorePerceptionComponent->HasActiveStimulus(*CurrentTargetActor, UAISense::GetSenseID<UAISense_Sight>()))
			{
				UpdateTargetState(CurrentTargetActor, GASCoreTags::State_AI_Combat);
			}
		}
	}
}

void ACoreAIController::UpdateTargetState(AActor* Target, FGameplayTag NewStateTag)
{
	if (!Target) return;

	bool bFound = false;

	// Find the target in memory and update its tag
	for (FPerceivedData& TargetData : KnownTargets)
	{
		if (TargetData.TargetActor == Target)
		{
			TargetData.DesiredStateTag = NewStateTag;
			bFound = true;
			break;
		}
	}

	// Re evaluate targets based on the new information
	if (bFound)
	{
		EvaluateBestTarget();
	}
}

void ACoreAIController::SetTarget(AActor* NewTarget)
{
	// Controllers local reference (needed for OnDetectionLevelChanged)
	CurrentTargetActor = NewTarget;

	// Pushing the target down to the Character for ABP replication purposes
	if (IsValid(ControlledCharacter))
	{
		ControlledCharacter->CurrentTargetActor = NewTarget;
	}
}

FGenericTeamId ACoreAIController::GetGenericTeamId() const
{
	// Ask the pawn we are currently possessing what team it is on
	if (const IGenericTeamAgentInterface* PawnTeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn()))
	{
		return PawnTeamAgent->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}
