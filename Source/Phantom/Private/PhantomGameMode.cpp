// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomGameMode.h"
#include "AmmoSpawnSubsystem.h"
#include "CollectibleSpawnSubsystem.h"
#include "EquipmentSpawnSubsystem.h"

APhantomGameMode::APhantomGameMode()
{
	bUseSeamlessTravel = true;
}

void APhantomGameMode::StartPlay()
{
	Super::StartPlay();
	
	InitCollectiblePickups();
	InitEquipmentPickups();
	InitAmmoPickups();
}

void APhantomGameMode::InitCollectiblePickups() const
{
	UCollectibleSpawnSubsystem* CollectibleSpawnSubsystem = GetWorld()->GetSubsystem<UCollectibleSpawnSubsystem>();
	
	if (!CollectibleSpawnSubsystem) return;
	
	if (!CollectibleDropTable) return;
	
	// Give the collectible spawner collectibles to populate the level with
	CollectibleSpawnSubsystem->InitializeCollectibleTables(
		CollectibleDropTable, RareCollectibleDropTable);
	
	// Spawn collectibles
	CollectibleSpawnSubsystem->SpawnCollectibles();
}

void APhantomGameMode::InitEquipmentPickups() const
{
	UEquipmentSpawnSubsystem* EquipmentSpawnSubsystem = GetWorld()->GetSubsystem<UEquipmentSpawnSubsystem>();
	
	if (!EquipmentSpawnSubsystem) return;
	
	if (!EquipmentDropTable) return;
	
	// Give the spawner equipment to populate the level with
	EquipmentSpawnSubsystem->InitializeEquipmentTable(EquipmentDropTable);
	
	// Spawn
	EquipmentSpawnSubsystem->SpawnEquipments();
}

void APhantomGameMode::InitAmmoPickups() const
{
	UAmmoSpawnSubsystem* AmmoSpawnSubsystem = GetWorld()->GetSubsystem<UAmmoSpawnSubsystem>();
	
	if (!AmmoSpawnSubsystem) return;
	
	if (!AmmoDropTable) return;
	
	// Give the spawner ammo to populate the level with
	AmmoSpawnSubsystem->InitializeAmmoTable(AmmoDropTable);
	
	// Spawn
	AmmoSpawnSubsystem->SpawnAmmos();
}