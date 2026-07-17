// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PickupEquipment.h"
#include "Components/EquipmentComponent.h"
#include "Data/EquipmentDefinition.h"

APickupEquipment::APickupEquipment()
{
    bReplicates = true;
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