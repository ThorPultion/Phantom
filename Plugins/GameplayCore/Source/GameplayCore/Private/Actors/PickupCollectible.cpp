// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PickupCollectible.h"
#include "Data/CollectibleDefinition.h"

void APickupCollectible::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority()) return;

	if (Interactor && ItemData)
	{
		if (const UCollectibleDefinition* CollectibleData = Cast<UCollectibleDefinition>(ItemData))
		{
			// Data asset defines how this collectible is collected
			CollectibleData->CollectCollectible(Interactor);
			Destroy();
		}
	}
}
