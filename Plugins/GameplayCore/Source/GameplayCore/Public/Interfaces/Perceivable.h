// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Perceivable.generated.h"

struct FAIStimulus;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UPerceivable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEPLAYCORE_API IPerceivable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** How the observer should react to the perceivable */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Perception")
	FGameplayTag GetPerceptionTag(ETeamAttitude::Type ObserverAttitude, const FAIStimulus& Stimulus);
	
	/** How visible this perceivable is */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Perception")
	float GetVisibilityModifier();
	
	/** The level of visibility (from 0.0-1.0, lower = darker) at which this perceivable is invisible to observers */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Perception")
	float GetInvisibleThreshold();
};
