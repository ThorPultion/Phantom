// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAIController.h"
#include "Components/StateTreeAIComponent.h"
#include "CoreAIPerceptionComponent.h"
#include "CoreAICharacter.h"
#include "AbilitySystemComponent.h"
#include "Perceivable.h"

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

void ACoreAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!IsValid(Actor)) return;

	// Get AIs own ASC
	ACoreAICharacter* ControlledAI = Cast<ACoreAICharacter>(GetPawn());
	if (!ControlledAI) return;

	UAbilitySystemComponent* AI_ASC = ControlledAI->GetAbilitySystemComponent();
	if (!AI_ASC) return;

	// Does perceived actor implement Perceivable?
	if (Actor->Implements<UPerceivable>())
	{
		// The sensed object will decide what Gameplay Effect to apply
		TSubclassOf<UGameplayEffect> ReactionGE = IPerceivable::Execute_GetPerceptionReactionEffect(Actor, Stimulus);

		if (ReactionGE)
		{
			FGameplayEffectContextHandle Context = AI_ASC->MakeEffectContext();
			Context.AddInstigator(ControlledAI, ControlledAI);

			AI_ASC->ApplyGameplayEffectToSelf(ReactionGE->GetDefaultObject<UGameplayEffect>(), 1.0f, Context);
		}
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