// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"
#include "EquipmentDefinition.h"
#include "CoreAbilitySystemComponent.h"
#include "WeaponBase.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"
#include "CoreCharacterBase.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    SetIsReplicatedByDefault(true);
	// ...
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Register the variables to replicate to all clients
    DOREPLIFETIME(UEquipmentComponent, Loadout);
    DOREPLIFETIME(UEquipmentComponent, CurrentSlotIndex);
    DOREPLIFETIME(UEquipmentComponent, CurrentWeapon);
}

void UEquipmentComponent::EquipItem(UEquipmentDefinition* EquipmentDef)
{
    if (!EquipmentDef || !EquipmentDef->WeaponActorClass) return;

    AActor* OwningActor = GetOwner();
    ACoreCharacterBase* OwningCharacter = Cast<ACoreCharacterBase>(OwningActor);
    if (!IsValid(OwningCharacter)) return;

    if (!OwningActor->HasAuthority()) return;

    UnequipItem();

    // Spawning weapon actor
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwningActor;
    SpawnParams.Instigator = OwningCharacter;

    CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(EquipmentDef->WeaponActorClass, SpawnParams);

    CurrentWeapon->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquipmentDef->AttachmentSocket);

    if (CurrentWeapon && CurrentWeapon->ThirdPersonMesh && OwningCharacter->GetMesh())
    {
        CurrentWeapon->ThirdPersonMesh->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquipmentDef->AttachmentSocket);
    }

    if (CurrentWeapon && CurrentWeapon->FirstPersonMesh && OwningCharacter->GetFirstPersonMesh())
    {
        CurrentWeapon->FirstPersonMesh->AttachToComponent(OwningCharacter->GetFirstPersonMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquipmentDef->AttachmentSocket);
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

void UEquipmentComponent::AddItemToLoadout(UEquipmentDefinition* NewItem)
{
    if (!GetOwner()->HasAuthority() || !NewItem) return;

    // Do we have space?
    if (Loadout.Num() < MaxLoadoutSlots)
    {
        Loadout.Add(NewItem);

        // If our hands are empty, auto-equip it!
        if (CurrentSlotIndex == -1)
        {
            EquipItemFromSlot(Loadout.Num() - 1);
        }
    }
    else
    {
        // Optional: Swap the current weapon for the one on the ground
    }
}

void UEquipmentComponent::EquipItemFromSlot(int32 SlotIndex)
{
    // 1. Validate the index
    if (!Loadout.IsValidIndex(SlotIndex)) return;

    // 2. Update our tracker
    CurrentSlotIndex = SlotIndex;

    // 3. Call the exact EquipItem() logic you already wrote!
    EquipItem(Loadout[CurrentSlotIndex]);
}

void UEquipmentComponent::EquipNextWeapon()
{
    if (!GetOwner()->HasAuthority() || Loadout.IsEmpty()) return;

    // Calculate the next index (loop back to 0 if we hit the end)
    int32 NextIndex = (CurrentSlotIndex + 1) % Loadout.Num();

    EquipItemFromSlot(NextIndex);
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

void UEquipmentComponent::OnRep_CurrentSlotIndex()
{
    // The server changed our weapon slot. 
    // This is where you would update the UI or play a weapon-swap sound on the client!
}

void UEquipmentComponent::OnRep_CurrentWeapon(AWeaponBase* OldWeapon)
{
    if (!CurrentWeapon) return;

    ACoreCharacterBase* OwningCharacter = Cast<ACoreCharacterBase>(GetOwner());
    if (!IsValid(OwningCharacter)) return;

    // Only the person playing the game needs to attach the First Person mesh
    if (OwningCharacter->IsLocallyControlled() && Loadout.IsValidIndex(CurrentSlotIndex))
    {
        if (UEquipmentDefinition* Def = Loadout[CurrentSlotIndex])
        {
            // 2. Client explicitly glues the FP Weapon Mesh to the local FP Arms!
            if (CurrentWeapon->FirstPersonMesh && OwningCharacter->GetFirstPersonMesh())
            {
                CurrentWeapon->FirstPersonMesh->AttachToComponent(OwningCharacter->GetFirstPersonMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Def->AttachmentSocket);
            }
        }
    }
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

    // 1. Only the Server sets up the initial inventory
    if (GetOwner()->HasAuthority())
    {
        // 2. Loop through our default items
        for (UEquipmentDefinition* StartingItem : DefaultLoadout)
        {
            if (StartingItem)
            {
                // 3. Funnel them right into the exact same logic we use for pickups!
                AddItemToLoadout(StartingItem);
            }
        }
    }
	
}


// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

