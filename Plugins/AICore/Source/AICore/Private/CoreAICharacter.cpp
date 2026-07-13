// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAICharacter.h"
#include "CoreAIController.h"
#include "AbilitySystemComponent.h"
#include "CoreAttributeSet.h"
#include "CoreAbilitySystemComponent.h"
#include "AIAttributeSet.h"
#include "AIGASInitData.h"
#include "GASCoreTags.h"
#include "Components/CoreWidgetComponent.h"

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

void ACoreAICharacter::InitAbilitySystem()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// IMPORTANT:
	// Calling base class function AFTER initializing, ASC needs to be set up in child classes such as this one FIRST!
	Super::InitAbilitySystem();

	CombatTagDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(
		GASCoreTags::State_AI_Combat,
		EGameplayTagEventType::AnyCountChange).AddUObject(this, &ACoreAICharacter::OnCombatTagChanged);
}

void ACoreAICharacter::OnCombatTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	FString DebugMsg = FString::Printf(TEXT("Combat Tag Count is now: %d"), NewCount);
	FColor MsgColor = (NewCount > 1) ? FColor::Red : FColor::Yellow;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, MsgColor, DebugMsg);

	// ONLY dump the stack trace when the tag illegally stacks!
	if (NewCount > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("=== ILLEGAL COMBAT TAG STACK DETECTED! COUNT: %d ==="), NewCount);
		FDebug::DumpStackTraceToLog(ELogVerbosity::Warning);
		UE_LOG(LogTemp, Warning, TEXT("================================================="));
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