// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIDetectionData.generated.h"

class UGameplayEffect;

/** Data for Detection attribute handling */
UCLASS(BlueprintType)
class AICORE_API UAIDetectionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Defines how high detection level needs to be for AI to enter search state (0.3 = 30% of MaxDetection) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifier", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SearchThresholdPercent = 0.3f;
	
	/** Maximum possible detection impulse by a sound event */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifier", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float MaxHearingDetection = 35.f;
	
	/** How fast we want continuous detection to build */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifier", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float DetectionBuildRate = 5.f;
	
	/** How far do we want to broadcast AIs Team events */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team", meta = (ClampMin = "0.0", ClampMax = "10000.0"))
	float TeamBroadcastRange = 1500.f;
	
	/** For AI perception events that should build detection level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DetectionBuildGEClass;
    
	/** Multiplier applied to detection build-up when the target is crouching */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stealth", meta = (ClampMin = "0.0"))
	float CrouchDetectionMultiplier = 0.5f;

	/** Multiplier applied to detection build-up when the target is in shadows/darkness */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stealth", meta = (ClampMin = "0.0"))
	float ShadowDetectionMultiplier = 0.3f;
};