// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "CoreInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FCoreInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	/** The Gameplay Tag broadcasted to the ASC when this Input Action is triggered */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;
};

UCLASS()
class PLAYERCORE_API UCoreInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	/** List of input actions mapped to Gameplay Tags, used by the custom Input Component to trigger abilities */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputAction"))
	TArray<FCoreInputAction> AbilityInputActions;
};