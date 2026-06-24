// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectileProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProjectileProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEPLAYCORE_API IProjectileProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** Get current active projectiles class */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile")
	TSubclassOf<AActor> GetCurrentProjectileClass() const;

	/** Get appropriate spawn position for projectile */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectile")
	FTransform GetProjectileSpawnTransform() const;
};
