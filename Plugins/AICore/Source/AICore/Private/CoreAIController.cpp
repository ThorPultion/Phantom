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
	CorePerceptionComponent = CreateDefaultSubobject<UCoreAIPerceptionComponent>(TEXT("PerceptionComponent"));

	SetPerceptionComponent(*CorePerceptionComponent);
	
	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
}

void ACoreAIController::BeginPlay()
{
	Super::BeginPlay();
	
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
	
	if (CorePerceptionComponent)
	{
		CorePerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACoreAIController::OnTargetPerceptionUpdated);
	}
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
			ProcessTargetSensed(Actor, ExistingData, ReactionTag);
		}
		else
		{
			ProcessTargetLost(Actor, ExistingData, ReactionTag, Stimulus);
		}
	}

	EvaluateBestTarget();
}

void ACoreAIController::ProcessTargetSensed(AActor* TargetActor, FPerceivedData* ExistingData, const FGameplayTag ReactionTag)
{
	if (ExistingData)
	{
		// ESCALATION: Only update if the new tag is a higher priority.
		// (For example, prevents a perception pulse from overwriting Combat with Suspicious)
		int32 CurrentScore = ControlledCharacter->PriorityData->StatePriorities.FindRef(ExistingData->DesiredStateTag);
		int32 ProposedScore = ControlledCharacter->PriorityData->StatePriorities.FindRef(ReactionTag);

		if (ProposedScore > CurrentScore)
		{
			ExistingData->DesiredStateTag = ReactionTag;
		}
				
		ExistingData->LastKnownLocation = TargetActor->GetActorLocation();
	}
	else
	{
		// Entirely new target, add them to memory
		FPerceivedData NewData;
		NewData.TargetActor = TargetActor;
		NewData.DesiredStateTag = ReactionTag;
		NewData.LastKnownLocation = TargetActor->GetActorLocation();
				
		KnownTargets.Add(NewData);
	}
}

void ACoreAIController::ProcessTargetLost(AActor* TargetActor, FPerceivedData* ExistingData, FGameplayTag ReactionTag,
	const FAIStimulus& Stimulus)
{
	if (ExistingData)
	{
		// If MaxAge on a sense expired
		if (Stimulus.IsExpired())
		{
			// The AI has forgotten this actor so we wipe all our info on the actor
			KnownTargets.RemoveAll([TargetActor](const FPerceivedData& Data) {
				return Data.TargetActor == TargetActor;
			});
					
			// We dont want to run anything else
			return;
		}

		// MULTI-SENSE CHECK: Only evaluate a downgrade if we have completely lost ALL active senses on this target
		if (!CorePerceptionComponent->HasAnyCurrentStimulus(*TargetActor))
		{
			ExistingData->LastKnownLocation = TargetActor->GetActorLocation();
                
			// Threshold check
			if (AbilitySystemComponent)
			{
				const float CurrentDetection = AbilitySystemComponent->GetNumericAttribute(UAIAttributeSet::GetDetectionLevelAttribute());
				const float MaxDetection = AbilitySystemComponent->GetNumericAttribute(UAIAttributeSet::GetMaxDetectionAttribute());
              
				const float InvestigationThreshold = MaxDetection * 0.3f;

				// If detection level is higher than our designated threshold when losing sight,
				// apply the searching tag
				if (CurrentDetection >= InvestigationThreshold)
				{
					ExistingData->DesiredStateTag = ReactionTag;
				}
				else
				{
					// The AIs detection level is too low to apply searching tag
					ExistingData->DesiredStateTag = GASCoreTags::State_AI_Routine;
				}
			}
			else
			{
				ExistingData->DesiredStateTag = ReactionTag;
			}
		}
	}
}

void ACoreAIController::EvaluateBestTarget()
{
	// LOCK: If we are already evaluating, flag that we need to evaluate again and return early.
	// This avoids shenanigans with tag removal and addition due to nested execution (re-entrancy),
	// AKA, when tag changes, this function is called again before finishing its first execution
	// which causes the rest of the code to fail
	if (bIsEvaluatingTargets)
	{
		bEvaluationPending = true;
		return;
	}

	// Locking the function
	bIsEvaluatingTargets = true;
	bEvaluationPending = false;
	
	// Checking cached pointers
	if (!IsValid(ControlledCharacter) || !ControlledCharacter->PriorityData)
	{
		CurrentTargetData = FPerceivedData(); 
		bIsEvaluatingTargets = false;
		return;
	}

	FPerceivedData BestTargetData;
	int32 HighestScore = -1;
	float ClosestDistanceSq = MAX_flt; // Tracking distance for tie breakers

	// Loop through targets to find highest scored tag
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
			BestTargetData = TargetData;
			ClosestDistanceSq = DistanceSq;
		}
		// TIE BREAKER: Are the priorities exactly the same?
		else if (Score == HighestScore)
		{
			// Pick the one that is physically closer
			if (DistanceSq < ClosestDistanceSq)
			{
				BestTargetData = TargetData;
				ClosestDistanceSq = DistanceSq;
			}
		}
	}

	const FGameplayTag NewTag = BestTargetData.DesiredStateTag;
	
	// Update ASC loose tags using the cached ASC
	if (CurrentTargetData.DesiredStateTag != NewTag)
	{
		// Cache the old tag
		const FGameplayTag OldTag = CurrentTargetData.DesiredStateTag;
		
		// Update data
		CurrentTargetData = BestTargetData;

		if (AbilitySystemComponent)
		{
			if (OldTag.IsValid())
			{
				AbilitySystemComponent->RemoveLooseGameplayTag(OldTag);
			}
			
			if (BestTargetData.DesiredStateTag.IsValid())
			{
				// If this triggers a GAS cascade that calls EvaluateBestTarget() again,
				// the nested call will hit the lock, set bEvaluationPending = true, and harmlessly return.
				AbilitySystemComponent->AddLooseGameplayTag(BestTargetData.DesiredStateTag);
			}
		}
	}
	else
	{
		// We still update data even if tag didnt change
		CurrentTargetData = BestTargetData;
	}

	SetTarget(CurrentTargetData.TargetActor);
	
	// Unlocking the function
	bIsEvaluatingTargets = false;
	
	// If an evaluation is pending due to an evaluation attempt occurring while the block was running, we run it again
	if (bEvaluationPending)
	{
		EvaluateBestTarget();
	}
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
	const FGenericTeamId OurTeam = GetGenericTeamId();
	const FGenericTeamId TheirTeam = OtherTeamAgent->GetGenericTeamId();

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
		if (CurrentTargetData.TargetActor)
		{
			// Update to combat state if we see the target
			if (CorePerceptionComponent->HasActiveStimulus(*CurrentTargetData.TargetActor, UAISense::GetSenseID<UAISense_Sight>()))
			{
				UpdateTargetState(CurrentTargetData.TargetActor, GASCoreTags::State_AI_Combat);
			}
		}
	}
	else if (Data.OldValue > 0.f && Data.NewValue <= 0.f)
	{
		if (!CorePerceptionComponent->HasActiveStimulus(*CurrentTargetData.TargetActor, UAISense::GetSenseID<UAISense_Sight>()))
		{
			UpdateTargetState(CurrentTargetData.TargetActor, GASCoreTags::State_AI_Routine);
		}
	}
}

void ACoreAIController::UpdateTargetState(AActor* Target, FGameplayTag NewStateTag)
{
	if (!IsValid(Target)) return;	
	
	if (FPerceivedData* ExistingData = KnownTargets.FindByKey(Target))
	{
		if (ExistingData->DesiredStateTag == NewStateTag)
		{
			return;
		}
		
		ExistingData->DesiredStateTag = NewStateTag;
		// Re-evaluate best target
		EvaluateBestTarget();
	}
}

void ACoreAIController::SetTarget(AActor* NewTarget) const
{
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

FVector ACoreAIController::GetFocalPointOnActor(const AActor* Actor) const
{
	if (Actor != nullptr)
	{
		// We force SetFocus to look at the exact socket thats defined on the targets end
		return Actor->GetTargetLocation(GetPawn());
	}

	return FAISystem::InvalidLocation;
}