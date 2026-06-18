// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAICharacter.h"
#include "CoreAIController.h"
#include "AbilitySystemComponent.h"
#include "CoreAttributeSet.h"
#include "CoreAbilitySystemComponent.h"

// Sets default values
ACoreAICharacter::ACoreAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UCoreAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UCoreAttributeSet>(TEXT("AttributeSet"));

	AIControllerClass = ACoreAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ACoreAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

// Called when the game starts or when spawned
void ACoreAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoreAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoreAICharacter::InitAbilitySystem()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

