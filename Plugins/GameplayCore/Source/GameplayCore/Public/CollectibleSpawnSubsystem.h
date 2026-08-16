// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CollectibleSpawnSubsystem.generated.h"

class ACollectibleSpawnPoint;
class UCollectibleDropTable;
/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UCollectibleSpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	/** Register a point for collectible spawning */
	UFUNCTION(BlueprintCallable, Category = "Collectibles")
	void RegisterCollectibleSpawnPoint(ACollectibleSpawnPoint* SpawnPoint, bool bIsRare);
	
	/** Give the subsystem tables of collectibles, necessary to do before spawning! */
	UFUNCTION(BlueprintCallable, Category = "Collectibles")
	void InitializeCollectibleTables(UCollectibleDropTable* InDropTable, UCollectibleDropTable* InRareDropTable);
	
	/** Populate the level with collectibles */
	UFUNCTION(BlueprintCallable, Category = "Collectibles")
	void SpawnCollectibles();
	
private:
	UPROPERTY()
	TArray<TObjectPtr<ACollectibleSpawnPoint>> SpawnPoints;

	UPROPERTY()
	TArray<TObjectPtr<ACollectibleSpawnPoint>> RareSpawnPoints;

	UPROPERTY()
	TObjectPtr<UCollectibleDropTable> DropTable;

	UPROPERTY()
	TObjectPtr<UCollectibleDropTable> RareDropTable;
	
	void SpawnCollectible(const ACollectibleSpawnPoint* SpawnPoint, UCollectibleDropTable* InDropTable) const;
};
