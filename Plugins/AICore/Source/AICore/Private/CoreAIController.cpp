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
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Team.h"
#include "AIDetectionData.h"

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

	if (AbilitySystemComponent)
	{
		DetectionDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	UAIAttributeSet::GetDetectionLevelAttribute()).AddUObject(this, &ACoreAIController::OnDetectionLevelChanged);
	}
	
	if (CorePerceptionComponent)
	{
		CorePerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACoreAIController::OnTargetPerceptionUpdated);
	}
	
	// Refresh cached Team ID in the Perception System
	// Necessary for Perception System because TeamID lives in character and thus doesnt exist immediately.
	// Perception system takes the empty TeamID and does not refresh it until told so.
	// Other way of dealing with this is having TeamID in controller, but
	// logically I feel characters have teams, not controllers.
	if (CorePerceptionComponent && GetWorld())
	{
		if (UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld()))
		{
			PerceptionSystem->UpdateListener(*CorePerceptionComponent);
		}
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

	KnownTargets.Empty();
	CurrentTargetData = FPerceivedData();
	
	ControlledCharacter = nullptr;
	AbilitySystemComponent = nullptr;
}

void ACoreAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!IsValid(Actor) || !Actor->Implements<UPerceivable>()) return;
	
	// The sensed object will decide what state tag to apply
	FGameplayTag ReactionTag = IPerceivable::Execute_GetPerceptionTag(Actor, GetTeamAttitudeTowards(*Actor), Stimulus);
	
	const FAISenseID SenseID = Stimulus.Type;
	// If we receive a Team stimulus
	if (SenseID == UAISense::GetSenseID<UAISense_Team>())
	{
		// Asking our own character (so the AI itself) what its Team Assist tag is
		if (IsValid(ControlledCharacter) && ControlledCharacter->ReactionData)
		{
			// Overriding earlier ReactionTag assignment
			ReactionTag = ControlledCharacter->ReactionData->TeamAssistTag;
		}
	}
	
	if (!ReactionTag.IsValid()) return;
	
	// Apply Escalation / De-escalation Rules
	if (Stimulus.WasSuccessfullySensed())
	{
		ProcessTargetSensed(Actor, ReactionTag, Stimulus.StimulusLocation);
	}
	else
	{
		ProcessTargetLost(Actor, ReactionTag, Stimulus);
	}

	EvaluateBestTarget();
	
	if (SenseID == UAISense::GetSenseID<UAISense_Sight>())
	{
		HandleSightSense(Actor, Stimulus);
	}
	else if (SenseID == UAISense::GetSenseID<UAISense_Hearing>())
	{
		HandleHearingSense(Actor, Stimulus);
	}
	else if (SenseID == UAISense::GetSenseID<UAISense_Damage>())
	{
		HandleDamageSense(Actor, Stimulus);
	}
	else if (SenseID == UAISense::GetSenseID<UAISense_Team>())
	{
		HandleTeamSense(Actor, Stimulus);
	}
}

void ACoreAIController::HandleSightSense(AActor* Actor, const FAIStimulus& Stimulus)
{
	if (!Stimulus.WasSuccessfullySensed()) return;
    
    // Check if the actor we are looking at is a friendly teammate
    if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Friendly)
    {
       // Check what tag this target gave us in KnownTargets
       if (const FPerceivedData* TargetData = KnownTargets.FindByKey(Actor))
       {
          // If the teammate gave a tag that signifies they are engaging with something
          if (TargetData->DesiredStateTag == ControlledCharacter->ReactionData->TeamAssistTag)
          {
             // Impulse due to noticing ally engaged
             if (AbilitySystemComponent && DetectionData)
             {
                const float MaxDetection = AbilitySystemComponent->GetNumericAttribute(UAIAttributeSet::GetMaxDetectionAttribute());
                ApplyDetectionImpulse(Actor, MaxDetection * DetectionData->SearchThresholdPercent);
             }

             // Extracting target info from ally
             if (const AAIController* AllyController = Cast<AAIController>(Actor->GetInstigatorController()))
             {
                if (const ACoreAIController* CoreAllyController = Cast<ACoreAIController>(AllyController))
                {
                   AActor* AllyTargetActor = CoreAllyController->CurrentTargetData.TargetActor;
                   const FVector ThreatLocation = CoreAllyController->CurrentTargetData.LastKnownLocation;
                   
                   if (IsValid(AllyTargetActor) && !ThreatLocation.IsZero())
                   {
                      // Pass the actual enemy actor and the allys tracked threat location into our own memory
                      ProcessTargetSensed(
                         AllyTargetActor, 
                         ControlledCharacter->ReactionData->TeamAssistTag, 
                         ThreatLocation
                      );
                   }
                }
             }
          }
       }
    }
}

void ACoreAIController::HandleHearingSense(AActor* Actor, const FAIStimulus& Stimulus) const
{
	if (!Stimulus.WasSuccessfullySensed()) return;

	// Detection amount based on strength of sound stimulus
	const float DetectionAmount = FMath::Clamp(Stimulus.Strength * DetectionData->MaxHearingDetection, 0, DetectionData->MaxHearingDetection);
	ApplyDetectionImpulse(Actor, DetectionAmount);
}

void ACoreAIController::HandleDamageSense(AActor* Actor, const FAIStimulus& Stimulus) const
{
	if (!Stimulus.WasSuccessfullySensed() || !AbilitySystemComponent) return;
	
	const float MaxDetection = AbilitySystemComponent->GetNumericAttribute(UAIAttributeSet::GetMaxDetectionAttribute());
	ApplyDetectionImpulse(Actor, MaxDetection);
}

void ACoreAIController::HandleTeamSense(AActor* Actor, const FAIStimulus& Stimulus) const
{
	if (!Stimulus.WasSuccessfullySensed() || !AbilitySystemComponent) return;
	
	const float MaxDetection = AbilitySystemComponent->GetNumericAttribute(UAIAttributeSet::GetMaxDetectionAttribute());
	ApplyDetectionImpulse(Actor, MaxDetection * DetectionData->SearchThresholdPercent);
}

void ACoreAIController::ApplyDetectionImpulse(AActor* InstigatorActor, const float DetectionAmount) const
{
	if (!AbilitySystemComponent || !IsValid(InstigatorActor) || DetectionAmount <= 0.f || !DetectionData)
	{
		return;
	}

	if (!DetectionData->DetectionBuildGEClass)
	{
		return;
	}

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddInstigator(InstigatorActor, InstigatorActor);
	Context.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DetectionData->DetectionBuildGEClass, 1.0f, Context);
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(GASCoreTags::Data_Magnitude_Detection, DetectionAmount);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ACoreAIController::ProcessTargetSensed(AActor* TargetActor, const FGameplayTag ReactionTag, const FVector& StimulusLocation)
{
	if (!IsValid(TargetActor) || !ReactionTag.IsValid()) return;
	
	if (FPerceivedData* ExistingTargetData = KnownTargets.FindByKey(TargetActor))
	{
		// ESCALATION: Only update if the new tag is a higher priority.
		// (For example, prevents a perception pulse from overwriting Combat with Suspicious)
		int32 CurrentScore = ControlledCharacter->PriorityData->StatePriorities.FindRef(ExistingTargetData->DesiredStateTag);
		int32 ProposedScore = ControlledCharacter->PriorityData->StatePriorities.FindRef(ReactionTag);

		if (ProposedScore > CurrentScore)
		{
			ExistingTargetData->DesiredStateTag = ReactionTag;
		}
				
		ExistingTargetData->LastKnownLocation = TargetActor->GetActorLocation();
	}
	else
	{
		// Entirely new target, add them to memory
		FPerceivedData NewTargetData;
		NewTargetData.TargetActor = TargetActor;
		NewTargetData.DesiredStateTag = ReactionTag;
		NewTargetData.LastKnownLocation = TargetActor->GetActorLocation();
				
		KnownTargets.Add(NewTargetData);
	}
}

void ACoreAIController::ProcessTargetLost(AActor* TargetActor, const FGameplayTag ReactionTag, const FAIStimulus& Stimulus)
{
	FPerceivedData* ExistingTargetData = KnownTargets.FindByKey(TargetActor);
	if (!ExistingTargetData) return;

	// MULTI-SENSE CHECK: Only evaluate a downgrade if we have completely lost ALL active senses on this target.
	// IMPORTANT!:
	// This essentially checks WasSuccessfullySensed() for each sense.
	// Continuous senses such as Sight return false if player is not visible, even if MaxAge has not run out.
	// Event based senses such as Hearing and Damage return true until MaxAge runs out.
	if (!CorePerceptionComponent->HasAnyCurrentStimulus(*TargetActor))
	{
		ExistingTargetData->LastKnownLocation = Stimulus.StimulusLocation;
                
		// Threshold check
		if (AbilitySystemComponent)
		{
			const float CurrentDetection = AbilitySystemComponent->GetNumericAttribute(UAIAttributeSet::GetDetectionLevelAttribute());
			const float MaxDetection = AbilitySystemComponent->GetNumericAttribute(UAIAttributeSet::GetMaxDetectionAttribute());
              
			const float SearchThreshold = MaxDetection * DetectionData->SearchThresholdPercent;

			// If detection level is higher than our designated threshold when losing sight,
			// apply the searching tag
			if (CurrentDetection >= SearchThreshold)
			{
				ExistingTargetData->DesiredStateTag = ReactionTag;
			}
			else
			{
				// The AIs detection level is too low to apply searching tag
				ExistingTargetData->DesiredStateTag = GASCoreTags::State_AI_Routine;
			}
		}
		else
		{
			ExistingTargetData->DesiredStateTag = ReactionTag;
		}
	}
	
	PruneTargets();
}

void ACoreAIController::PruneTargets()
{
	KnownTargets.RemoveAll([this](const FPerceivedData& Data) {
		if (!IsValid(Data.TargetActor)) return true;

		// Checking if MaxAge has expired on all stimuli
		const FActorPerceptionInfo* ActorInfo = CorePerceptionComponent->GetActorInfo(*Data.TargetActor);
		const bool bHasActivePerception = ActorInfo && ActorInfo->HasAnyKnownStimulus();

		// Checking if detection is 0 or active.
		// We only check the detection when dealing with our currently focused target.
		// We default to false for non-current targets
		bool bHasActiveDetection = false;
		if (AbilitySystemComponent && CurrentTargetData.TargetActor == Data.TargetActor)
		{
		   const float CurrentDetection = AbilitySystemComponent->GetNumericAttribute(UAIAttributeSet::GetDetectionLevelAttribute());
		   bHasActiveDetection = (CurrentDetection > 0.f);
		}

		// If we have no perception AND no detection, delete target
		return !bHasActivePerception && !bHasActiveDetection;
	});
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
	
	// If all valid targets have been removed due to MaxAge running out on all senses,
	// clear target and go back to routine
	if (!BestTargetData.DesiredStateTag.IsValid())
	{
		BestTargetData.DesiredStateTag = GASCoreTags::State_AI_Routine;
		BestTargetData.TargetActor = nullptr;
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
			UpdateTargetState(CurrentTargetData.TargetActor, GASCoreTags::State_AI_Combat);
		}
	}
	else if (Data.OldValue > 0.f && Data.NewValue <= 0.f)
	{
		if (CurrentTargetData.TargetActor)
		{
			if (!CorePerceptionComponent->HasActiveStimulus(*CurrentTargetData.TargetActor, UAISense::GetSenseID<UAISense_Sight>()))
			{
				UpdateTargetState(CurrentTargetData.TargetActor, GASCoreTags::State_AI_Routine);
			}
		}
		
		PruneTargets();
		EvaluateBestTarget();
	}
}

void ACoreAIController::UpdateTargetState(AActor* Target, FGameplayTag NewStateTag)
{
	if (!IsValid(Target)) return;	
	
	if (FPerceivedData* ExistingTargetData = KnownTargets.FindByKey(Target))
	{
		if (ExistingTargetData->DesiredStateTag == NewStateTag)
		{
			return;
		}
		
		ExistingTargetData->DesiredStateTag = NewStateTag;
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

float ACoreAIController::GetSearchThreshold() const
{
	if (!AbilitySystemComponent || !DetectionData)
	{
		return 0.0f;
	}

	const float MaxDetection = AbilitySystemComponent->GetNumericAttribute(UAIAttributeSet::GetMaxDetectionAttribute());
	return MaxDetection * DetectionData->SearchThresholdPercent;
}