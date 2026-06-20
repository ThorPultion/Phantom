// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"
#include "EquipmentDefinition.h"
#include "CoreAbilitySystemComponent.h"
#include "WeaponBase.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UEquipmentComponent::EquipItem(UEquipmentDefinition* EquipmentDef)
{
    if (!EquipmentDef || !EquipmentDef->WeaponActorClass) return;

    AActor* OwningActor = GetOwner();
    ACharacter* OwningCharacter = Cast<ACharacter>(OwningActor);
    if (!IsValid(OwningCharacter)) return;

    if (!OwningActor->HasAuthority()) return;

    UnequipItem();

    // Spawning weapon actor
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwningActor;
    SpawnParams.Instigator = OwningCharacter;

    CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(EquipmentDef->WeaponActorClass, SpawnParams);

    if (CurrentWeapon && CurrentWeapon->ThirdPersonMesh && OwningCharacter->GetMesh())
    {
        CurrentWeapon->ThirdPersonMesh->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquipmentDef->AttachmentSocket);
    }

    if (CurrentWeapon && CurrentWeapon->FirstPersonMesh && OwningCharacter->GetMesh())
    {
        CurrentWeapon->FirstPersonMesh->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquipmentDef->AttachmentSocket);
    }

    // Granting abilities
    if (UCoreAbilitySystemComponent* CoreASC = GetOwnerASC())
    {
        if (EquipmentDef->GrantedAbilitySet)
        {
            // CoreASC handles granting
            CoreASC->GrantAbilitySetAsync(EquipmentDef->GrantedAbilitySet);

            CurrentEquipmentDefinition = EquipmentDef;
        }
    }
}

void UEquipmentComponent::UnequipItem()
{
    AActor* OwningActor = GetOwner();

    if (!IsValid(OwningActor)) return;

    // Cleanup should also only be driven by the server
    if (!OwningActor->HasAuthority()) return;

    // Remove abilities
    if (CurrentEquipmentDefinition && CurrentEquipmentDefinition->GrantedAbilitySet)
    {
        if (UCoreAbilitySystemComponent* CoreASC = GetOwnerASC())
        {
            // CoreASC handles removing
            CoreASC->RemoveAbilitySet(CurrentEquipmentDefinition->GrantedAbilitySet);
        }
    }

    // Destroy physical weapon actor
    if (CurrentWeapon)
    {
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;
    }

    CurrentEquipmentDefinition = nullptr;
}

UCoreAbilitySystemComponent* UEquipmentComponent::GetOwnerASC() const
{
    // Get the Actor this component is attached to
    AActor* OwningActor = GetOwner();
    if (!IsValid(OwningActor)) return nullptr;

    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwningActor))
    {
        // Grab the base ASC from the interface, and cast it to our custom one
        return Cast<UCoreAbilitySystemComponent>(ASI->GetAbilitySystemComponent());
    }

    return nullptr;
}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

