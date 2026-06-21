// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupEquipment.h"
#include "EquipmentComponent.h"
#include "EquipmentDefinition.h" // Assuming this is where your definition lives

APickupEquipment::APickupEquipment()
{
    SetReplicates(true);
}

void APickupEquipment::Interact_Implementation(AActor* Interactor)
{
    // Only server manages inventory
    if (!HasAuthority()) return;

    if (Interactor && ItemData)
    {
        if (UEquipmentDefinition* EquipmentDefinition = Cast<UEquipmentDefinition>(ItemData))
        {
            if (UEquipmentComponent* EquipmentComponent = Interactor->FindComponentByClass<UEquipmentComponent>())
            {
                EquipmentComponent->AddItemToLoadout(EquipmentDefinition);

                Destroy();

                return;
            }
        }
    }
}