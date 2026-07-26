// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AIReactionData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class GAMEPLAYCORE_API UAIReactionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/** The tag to apply when the AI successfully spots this alive target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reactions")
	FGameplayTag SensedAliveTag;

	/** The tag to apply when the AI loses sight of this target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reactions")
	FGameplayTag LostSightTag;

	/** The tag to apply when the AI spots this target and it has the Dead tag */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reactions")
	FGameplayTag SensedDeadTag;
};
