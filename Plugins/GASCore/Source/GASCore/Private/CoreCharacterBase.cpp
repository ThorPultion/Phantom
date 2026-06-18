// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "CoreAbilitySet.h"
#include "CoreAbilitySystemComponent.h"

// Sets default values
ACoreCharacterBase::ACoreCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

UAbilitySystemComponent* ACoreCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACoreCharacterBase::GiveStartingAbilities()
{
    UCoreAbilitySystemComponent* ASC = Cast<UCoreAbilitySystemComponent>(GetAbilitySystemComponent());

    if (!HasAuthority() || !ASC || !StartingAbilities) return;

	ASC->GrantAbilitySetAsync(StartingAbilities);
}

// Called when the game starts or when spawned
void ACoreCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

void ACoreCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Running on the Server when the controller takes possession
	InitAbilitySystem();

	// ASC is initiated, so give starting abilities
	GiveStartingAbilities();
}

// Called every frame
void ACoreCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACoreCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACoreCharacterBase::InitAbilitySystem()
{
	// Default empty implementation
}

