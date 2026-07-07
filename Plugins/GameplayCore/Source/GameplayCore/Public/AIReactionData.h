// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIReactionData.generated.h"

class UGameplayEffect;

/**
 * 
 */
UCLASS(BlueprintType)
class GAMEPLAYCORE_API UAIReactionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/** The effect to apply when the AI successfully spots this alive target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reactions")
	TSubclassOf<UGameplayEffect> SensedAliveEffect;

	/** The effect to apply when the AI loses sight of this target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reactions")
	TSubclassOf<UGameplayEffect> LostSightEffect;

	/** The effect to apply when the AI spots this target and it has the Dead tag */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reactions")
	TSubclassOf<UGameplayEffect> SensedDeadEffect;
};
