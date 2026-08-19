// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CollectibleDropTable.generated.h"

class UCollectibleDefinition;
class APickupCollectible;

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UCollectibleDropTable : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/** The table of collectibles */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collectibles")
	TArray<TObjectPtr<UCollectibleDefinition>> CollectibleDefinitions;
	
	/** Collectible class (rare or regular pickup?) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<APickupCollectible> CollectibleClassToSpawn;
};
