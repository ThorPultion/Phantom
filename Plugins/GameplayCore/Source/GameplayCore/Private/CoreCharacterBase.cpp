// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "CoreAbilitySet.h"
#include "CoreAbilitySystemComponent.h"
#include "EquipmentComponent.h"
#include "CoreAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GASCoreTags.h"

// Sets default values
ACoreCharacterBase::ACoreCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentManager"));
	EquipmentComponent->SetIsReplicated(true);
}

UAbilitySystemComponent* ACoreCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACoreCharacterBase::GrantStartingAbilities()
{
    UCoreAbilitySystemComponent* ASC = Cast<UCoreAbilitySystemComponent>(GetAbilitySystemComponent());

    if (!HasAuthority() || !ASC || !StartingAbilities) return;

	ASC->GrantAbilitySetAsync(StartingAbilities);
}

void ACoreCharacterBase::GrantStartingEquipment()
{
	UCoreAbilitySystemComponent* ASC = Cast<UCoreAbilitySystemComponent>(GetAbilitySystemComponent());

	if (!HasAuthority() || !ASC || !EquipmentComponent || EquipmentComponent->StartingEquipment.IsEmpty()) return;

	for (UEquipmentDefinition* StartingItem : EquipmentComponent->StartingEquipment)
	{
		if (StartingItem)
		{
			EquipmentComponent->AddItemToLoadout(StartingItem);
		}
	}
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
	GrantStartingAbilities();

	// Equipment grant abilities as well
	GrantStartingEquipment();
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
	// This Super should be called AFTER initializing ASC!
	if (AbilitySystemComponent && AttributeSet)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetMovementSpeedAttribute()).Remove(MovementSpeedChangedDelegateHandle);

		// Start listening for movement speed changes and store handle for cleanup
		MovementSpeedChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetMovementSpeedAttribute()).AddUObject(this, &ACoreCharacterBase::OnMovementSpeedChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(
			GASCoreTags::State_Weapon_Priming,
			EGameplayTagEventType::NewOrRemoved).Remove(AimingTagDelegateHandle);

		// Start listening for aiming GE and store handle for cleanup
		AimingTagDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(
			GASCoreTags::State_Weapon_Priming,
			EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ACoreCharacterBase::OnAimingTagChanged);
	}
}

void ACoreCharacterBase::OnMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	// Apply the new GAS value directly to the engine's movement component
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = Data.NewValue;
	}
}

void ACoreCharacterBase::OnAimingTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// If the count is > 0, the GE is active.
	bIsAiming = (NewCount > 0);
}

