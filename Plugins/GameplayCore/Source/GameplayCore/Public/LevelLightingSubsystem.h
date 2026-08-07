// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LevelLightingSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API ULevelLightingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
    /** 
     * How dark the directional light of this scene is at its brightest
     * or how dark the scene is in general.
     * 
     * This multiplier offsets the darkness of the level. This should be set PER LEVEL.
     * Used by AI sight detection.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float LevelBrightnessMultiplier = 20.0f;
    
};
