// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AmmoData.generated.h"

class ACoreProjectile;
class UNiagaraSystem;

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
};
