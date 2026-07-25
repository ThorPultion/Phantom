// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/GASInitData.h"
#include "AIGASInitData.generated.h"

/** Data for AI specific GAS Initialization
 * (regular GASInit runs on inherited base character, AIGASInit adds AI specific initialization) */
UCLASS()
class AICORE_API UAIGASInitData : public UGASInitData
{
	GENERATED_BODY()

public:

	/** Raw stat for SetByCaller magnitude for AIs attribute initialization */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxDetection = 100.f;

	/** Initialization handling GE */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialization")
	TSubclassOf<UGameplayEffect> AIInitializationEffect;
};