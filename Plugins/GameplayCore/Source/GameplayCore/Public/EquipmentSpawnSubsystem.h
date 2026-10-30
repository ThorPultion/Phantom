// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EquipmentSpawnSubsystem.generated.h"

class AEquipmentSpawnPoint;
class UItemDropTable;

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UEquipmentSpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	/**  Register a point for spawning */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void RegisterEquipmentSpawnPoint(AEquipmentSpawnPoint* SpawnPoint);
	
	/** Give the subsystem a table of items, necessary to do before spawning! */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void InitializeEquipmentTable(UItemDropTable* InDropTable);
	
	/** Populate the level with equipment */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SpawnEquipments();
	
private:
	UPROPERTY()
	TArray<TObjectPtr<AEquipmentSpawnPoint>> SpawnPoints;

	UPROPERTY()
	TObjectPtr<UItemDropTable> DropTable;
	
	void SpawnEquipment(const AEquipmentSpawnPoint* SpawnPoint, UItemDropTable* InDropTable) const;
};
