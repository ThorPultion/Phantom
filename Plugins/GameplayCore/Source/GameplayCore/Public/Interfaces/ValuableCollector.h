// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ValuableCollector.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UValuableCollector : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEPLAYCORE_API IValuableCollector
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	/** 
	 * Called when the actor collects a valuable. 
	 * BlueprintNativeEvent allows implementation in both C++ and Blueprints.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Valuables")
	void AddValuable(float GoldAmount);
};
