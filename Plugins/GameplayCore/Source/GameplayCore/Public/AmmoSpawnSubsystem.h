// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AmmoSpawnSubsystem.generated.h"

class AAmmoSpawnPoint;
class UItemDropTable;
/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UAmmoSpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	/** Register a point for spawning */
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void RegisterAmmoSpawnPoint(AAmmoSpawnPoint* SpawnPoint);
	
	/** Give the subsystem a table of items, necessary to do before spawning! */
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void InitializeAmmoTable(UItemDropTable* InDropTable);
	
	/** Populate the level with ammo */
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void SpawnAmmos();
	
private:
	UPROPERTY()
	TArray<TObjectPtr<AAmmoSpawnPoint>> SpawnPoints;

	UPROPERTY()
	TObjectPtr<UItemDropTable> DropTable;
	
	void SpawnAmmo(const AAmmoSpawnPoint* SpawnPoint, UItemDropTable* InDropTable) const;
};
