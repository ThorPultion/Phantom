// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Interactable.h"
#include "ItemDefinition.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

    /** What is the item called? */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
    FText ItemName;

    /** Defines what interaction verb is used when displaying interaction UI */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
    EInteractionType InteractionType = EInteractionType::Pickup;

    /** Custom interaction verb (eg. "Play" or "Peek" or "Craft") */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (EditCondition = "InteractionType == EInteractionType::Custom", EditConditionHides))
    FText CustomInteractionVerb;

    /** Mesh that pickups use when placed in world */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
    class UStaticMesh* PickupMesh;

    /** Items can call this to display interaction verb with item name on UI */
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    FText GetVerbText() const;

    /** Inventory or HUD icon */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
    class UTexture2D* ItemIcon;

    /** Description of item */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
    FText ItemDescription;

    // Can add stats here such as:
    // UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    // float Weight = 1.0f;
};
