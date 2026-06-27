// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"
#include "EquipmentDefinition.h"
#include "CoreAbilitySystemComponent.h"
#include "EquipmentBase.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"
#include "CoreCharacterBase.h"
#include "ProjectileProvider.h"
#include "AmmoCycler.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
	// ...
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UEquipmentComponent, Loadout);
    DOREPLIFETIME(UEquipmentComponent, CurrentSlotIndex);
    DOREPLIFETIME(UEquipmentComponent, CurrentItem);
}

void UEquipmentComponent::EquipItem(UEquipmentDefinition* EquipmentDefinition, int32 SlotIndex)
{
    if (!EquipmentDefinition || !EquipmentDefinition->EquipmentActorClass) return;

    AActor* OwningActor = GetOwner();
    ACoreCharacterBase* OwningCharacter = Cast<ACoreCharacterBase>(OwningActor);
    if (!IsValid(OwningCharacter)) return;

    if (!OwningActor->HasAuthority()) return;

    // Clean up previous item
    UnequipItem();

    // Update active slot tracker
    CurrentSlotIndex = SlotIndex;

    // Spawning weapon actor
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwningActor;
    SpawnParams.Instigator = OwningCharacter;

    CurrentItem = GetWorld()->SpawnActor<AEquipmentBase>(EquipmentDefinition->EquipmentActorClass, SpawnParams);

    // Attaching weapon BP
    CurrentItem->AttachToComponent(
        OwningCharacter->GetMesh(),
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        CurrentItem->AttachmentSocket);

    // No need to attach first person if not local player
    if (OwningCharacter->IsLocallyControlled() && CurrentItem && CurrentItem->FirstPersonMesh && OwningCharacter->GetFirstPersonMesh())
    {
        CurrentItem->FirstPersonMesh->AttachToComponent(
            OwningCharacter->GetFirstPersonMesh(),
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            CurrentItem->AttachmentSocket);
    }

    // Granting abilities
    if (UCoreAbilitySystemComponent* CoreASC = GetOwnerASC())
    {
        if (EquipmentDefinition->GrantedAbilitySet)
        {
            // CoreASC handles granting
            CoreASC->GrantAbilitySetAsync(EquipmentDefinition->GrantedAbilitySet);

            CurrentEquipmentDefinition = EquipmentDefinition;
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
    if (CurrentItem)
    {
        CurrentItem->Destroy();
        CurrentItem = nullptr;
    }

    CurrentEquipmentDefinition = nullptr;

    // Mark our hands empty
    CurrentSlotIndex = -1;
}

void UEquipmentComponent::Server_UnequipItem_Implementation()
{
    UnequipItem();
}

void UEquipmentComponent::AddItemToLoadout(UEquipmentDefinition* NewItem)
{
    if (!GetOwner()->HasAuthority() || !NewItem) return;

    // Do we have space?
    if (Loadout.Num() < MaxLoadoutSlots)
    {
        Loadout.Add(NewItem);

        // If our hands are empty, equip item
        if (CurrentSlotIndex == -1)
        {
            EquipItemFromSlot(Loadout.Num() - 1);
        }
    }
    else
    {
        // Optionally Swap the current weapon for the one on the ground
    }
}

void UEquipmentComponent::EquipItemFromSlot(int32 SlotIndex)
{
    if (!GetOwner()->HasAuthority()) return;

    // Make sure item exists on index
    if (!Loadout.IsValidIndex(SlotIndex)) return;

    EquipItem(Loadout[SlotIndex], SlotIndex);
}

void UEquipmentComponent::Server_EquipItemFromSlot_Implementation(int32 SlotIndex)
{
    EquipItemFromSlot(SlotIndex);
}

void UEquipmentComponent::EquipNextItem()
{
    if (!GetOwner()->HasAuthority() || Loadout.IsEmpty()) return;

    // Next index or loop back to start
    int32 NextIndex = (CurrentSlotIndex + 1) % Loadout.Num();

    EquipItemFromSlot(NextIndex);
}

void UEquipmentComponent::Server_EquipNextItem_Implementation()
{
    EquipNextItem();
}

UCoreAbilitySystemComponent* UEquipmentComponent::GetOwnerASC() const
{
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

void UEquipmentComponent::OnRep_CurrentItem(AEquipmentBase* OldItem)
{
    if (!CurrentItem) return;

    ACoreCharacterBase* OwningCharacter = Cast<ACoreCharacterBase>(GetOwner());
    if (!IsValid(OwningCharacter)) return;

    // Only the local player needs first person mesh setup
    if (OwningCharacter->IsLocallyControlled())
    {
        // Local player client attaches first person mesh OnRep of weapon change
        if (CurrentItem->FirstPersonMesh && OwningCharacter->GetFirstPersonMesh())
        {
            CurrentItem->FirstPersonMesh->AttachToComponent(
                OwningCharacter->GetFirstPersonMesh(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                CurrentItem->AttachmentSocket);
        }
    }
}

void UEquipmentComponent::CycleActiveItemAmmo(int32 Direction)
{
    if (CurrentItem && CurrentItem->Implements<UAmmoCycler>())
    {
        IAmmoCycler::Execute_CycleAmmo(CurrentItem, Direction);
    }
}

void UEquipmentComponent::SetItemAmmoIndex(int32 AmmoIndex)
{
    if (CurrentItem && CurrentItem->Implements<UAmmoCycler>())
    {
        IAmmoCycler::Execute_SetAmmoIndex(CurrentItem, AmmoIndex);
    }
}

void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

    // Granting starting items
    if (GetOwner()->HasAuthority())
    {
        for (UEquipmentDefinition* StartingItem : DefaultLoadout)
        {
            if (StartingItem)
            {
                AddItemToLoadout(StartingItem);
            }
        }
    }
	
}