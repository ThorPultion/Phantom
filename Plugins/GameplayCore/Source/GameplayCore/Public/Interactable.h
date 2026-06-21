// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
    Pickup      UMETA(DisplayName = "Pick Up"),
    Open        UMETA(DisplayName = "Open"),
    Read        UMETA(DisplayName = "Read"),
    Use         UMETA(DisplayName = "Use"),
    Custom      UMETA(DisplayName = "Custom (Type Raw Text Below)")
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEPLAYCORE_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

    // -------------------------------------------------------------------
    // BlueprintNativeEvent allows us to override this in C++ OR Blueprint.
    // It passes the Actor who initiated the interaction so the item knows who to talk back to.
    // -------------------------------------------------------------------
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void Interact(AActor* Interactor);

    // -------------------------------------------------------------------
    // Returns the text to display on the UI (e.g., "Press F to Pick Up M4A1", or "Open Door")
    // -------------------------------------------------------------------
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    FText GetInteractText();

};
