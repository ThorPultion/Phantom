// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AmmoData.generated.h"

class ACoreProjectile;
class UNiagaraSystem;
class UGameplayEffect;

/**
 * 
 */
UCLASS(BlueprintType)
class GAMEPLAYCORE_API UAmmoData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	// Could make mesh and NS soft pointers
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
	TSubclassOf<ACoreProjectile> AmmoClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
	TObjectPtr<UStaticMesh> AmmoMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
	TObjectPtr<UNiagaraSystem> AmmoParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TMap<FGameplayTag, float> DamageConfig;

	// The Effect to apply on impact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UGameplayEffect> ImpactDamageEffect;

	// The Cue to trigger for impact visuals/audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FGameplayTag ImpactGameplayCue;
};
