// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AmmoCycler.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAmmoCycler : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEPLAYCORE_API IAmmoCycler
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ammo")
	void CycleAmmo(int32 Direction);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ammo")
	void SetAmmoIndex(int32 AmmoIndex);
};