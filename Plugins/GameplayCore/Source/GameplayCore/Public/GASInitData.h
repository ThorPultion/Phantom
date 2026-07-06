// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GASInitData.generated.h"

class UCoreAbilitySet;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UGASInitData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Set of GAs that will be async loaded
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	UCoreAbilitySet* StartingAbilities;

	// GE used to apply stats
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> InitializationEffect;

	// The raw stats for SetByCaller magnitudes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxEnergy = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> PassiveEffects;
};
