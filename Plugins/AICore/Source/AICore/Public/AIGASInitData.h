// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASInitData.h"
#include "AIGASInitData.generated.h"

/**
 * 
 */
UCLASS()
class AICORE_API UAIGASInitData : public UGASInitData
{
	GENERATED_BODY()

public:

	// The raw stat for SetByCaller magnitude for AI perception
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxDetection = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialization")
	TSubclassOf<UGameplayEffect> AIInitializationEffect;
};