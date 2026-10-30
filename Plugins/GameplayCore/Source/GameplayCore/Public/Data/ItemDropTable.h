// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDropTable.generated.h"

class UItemDefinition;
class APickupBase;

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UItemDropTable : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/** The table of items */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	TArray<TObjectPtr<UItemDefinition>> ItemDefinitions;
	
	/** What class of the item to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<APickupBase> ItemClassToSpawn;
};
