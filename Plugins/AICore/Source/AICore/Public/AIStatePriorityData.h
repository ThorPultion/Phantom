// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AIStatePriorityData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UAIStatePriorityData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Priorities")
    TMap<FGameplayTag, int32> StatePriorities;
};