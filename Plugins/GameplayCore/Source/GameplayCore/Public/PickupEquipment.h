// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "PickupEquipment.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API APickupEquipment : public APickupBase
{
	GENERATED_BODY()
	
public:
	virtual void Interact_Implementation(AActor* Interactor) override;
};
