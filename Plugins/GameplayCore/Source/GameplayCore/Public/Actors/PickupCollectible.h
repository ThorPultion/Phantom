// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupBase.h"
#include "PickupCollectible.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API APickupCollectible : public APickupBase
{
	GENERATED_BODY()
	
public:
	APickupCollectible();
	
	virtual void Interact_Implementation(AActor* Interactor) override;
};
