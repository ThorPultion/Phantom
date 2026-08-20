// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAICharacter.h"
#include "CoreAIController.h"
#include "AbilitySystemComponent.h"
#include "CoreAttributeSet.h"
#include "CoreAbilitySystemComponent.h"
#include "AIAttributeSet.h"
#include "AIDetectionData.h"
#include "AIGASInitData.h"
#include "GASCoreTags.h"
#include "Components/CoreWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionTypes.h"

// Sets default values
ACoreAICharacter::ACoreAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UCoreAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UCoreAttributeSet>(TEXT("AttributeSet"));
	AIAttributeSet = CreateDefaultSubobject<UAIAttributeSet>(TEXT("AIAttributeSet"));

	AIControllerClass = ACoreAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	DetectionWidgetComponent = CreateDefaultSubobject<UCoreWidgetComponent>(TEXT("DetectionWidget"));
	DetectionWidgetComponent->SetupAttachment(RootComponent);
	DetectionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // Screen space makes it always face the player
	DetectionWidgetComponent->SetDrawAtDesiredSize(true);
	DetectionWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
}

void ACoreAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

// Called when the game starts or when spawned
void ACoreAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// THIS IS SUSPECT. CURRENTLY USED TO REPLICATE ABP AIM ANIMATION DUE TO GETTING AIMING TAG FROM GE
	// MIGHT CAUSE ISSUES? RACE CONDITIONS?
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		InitAbilitySystem();
	}
}

// Called every frame
void ACoreAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoreAICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACoreAICharacter, CurrentTargetActor);
}

void ACoreAICharacter::InitAbilitySystem()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// IMPORTANT:
	// Calling base class function AFTER initializing, ASC needs to be set up in child classes such as this one FIRST!
	Super::InitAbilitySystem();

	CombatTagDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(
		GASCoreTags::State_Dead,
		EGameplayTagEventType::AnyCountChange).AddUObject(this, &ACoreAICharacter::OnDeadTagChanged);
}

void ACoreAICharacter::OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::OnDeadTagChanged(CallbackTag, NewCount);
	
	if (NewCount > 0)
	{
		DetectionWidgetComponent->DestroyComponent();
	}
}

void ACoreAICharacter::SetupAttributes()
{
	// Base class initializes basic attributes
	Super::SetupAttributes();

	if (!HasAuthority() || !AbilitySystemComponent) return;

	// Initializing AI specific stats
	if (UAIGASInitData* AIData = Cast<UAIGASInitData>(GASInitData))
	{
		if (AIData->AIInitializationEffect)
		{
			FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
			ContextHandle.AddSourceObject(GASInitData);

			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(AIData->AIInitializationEffect, 1.0f, ContextHandle);

			if (SpecHandle.IsValid())
			{
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(GASCoreTags::Data_Attribute_MaxDetection, AIData->MaxDetection);
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

FGameplayTag ACoreAICharacter::GetPerceptionTag_Implementation(ETeamAttitude::Type ObserverAttitude, const FAIStimulus& Stimulus)
{
	// If we are dead
	if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(GASCoreTags::State_Dead))
	{
		return Stimulus.WasSuccessfullySensed() ? ReactionData->SensedDeadTag : FGameplayTag::EmptyTag;
	}

	if (ObserverAttitude == ETeamAttitude::Hostile)
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			return ReactionData->SensedAliveTag;
		}
		else
		{
			return ReactionData->LostSightTag;
		}
	}
	
	// Currently the states detected for friendlies are only applied for AIs,
	// so for example a friendly AI assisting a player is not yet supported.
	if (ObserverAttitude == ETeamAttitude::Friendly)
	{
		// If we arent successfully sensed by our teammate, ignore
		if (!Stimulus.WasSuccessfullySensed())
		{
			return FGameplayTag::EmptyTag;
		}

		// Check if we (the observed character) are currently in combat or active state
		if (AbilitySystemComponent && ReactionData)
		{
			// If we are actively fighting or alerted, tell our teammates to assist us
			if (AbilitySystemComponent->HasMatchingGameplayTag(GASCoreTags::State_AI_Combat) || 
				AbilitySystemComponent->HasMatchingGameplayTag(GASCoreTags::State_AI_Searching))
			{
				return ReactionData->TeamAssistTag;
			}
			
			// Conditional Assist (Suspicious + High Detection)
			if (AbilitySystemComponent->HasMatchingGameplayTag(GASCoreTags::State_AI_Suspicious))
			{
				if (const ACoreAIController* CoreAIController = Cast<ACoreAIController>(GetController()))
				{
					if (AIAttributeSet && 
						AIAttributeSet->GetDetectionLevel() >= 
						AIAttributeSet->GetMaxDetection() * CoreAIController->DetectionData->SearchThresholdPercent)
					{
						return ReactionData->TeamAssistTag;
					}
				}
			}
		}
	}
	
	return FGameplayTag::EmptyTag;
}