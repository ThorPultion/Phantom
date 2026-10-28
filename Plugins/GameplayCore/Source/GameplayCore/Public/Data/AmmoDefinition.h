// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemDefinition.h"
#include "AmmoDefinition.generated.h"

class UAmmoData;
/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UAmmoDefinition : public UItemDefinition
{
	GENERATED_BODY()
	
public:
	/** The Ammo to be added to the EquipmentComponent when this is picked up */ 
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	TObjectPtr<UAmmoData> GrantedAmmo;
};
