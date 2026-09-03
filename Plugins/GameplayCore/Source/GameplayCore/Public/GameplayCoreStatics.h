// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayCoreStatics.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UGameplayCoreStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Version")
	static FString GetProjectVersion();
	
};
