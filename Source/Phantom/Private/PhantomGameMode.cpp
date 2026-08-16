// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomGameMode.h"
#include "CollectibleSpawnSubsystem.h"

void APhantomGameMode::StartPlay()
{
	Super::StartPlay();
	
	InitCollectibles();
}

void APhantomGameMode::InitCollectibles() const
{
	UCollectibleSpawnSubsystem* CollectibleSpawnSubsystem = GetWorld()->GetSubsystem<UCollectibleSpawnSubsystem>();
	
	if (!CollectibleSpawnSubsystem) return;
	
	// Give the collectible spawner collectibles to populate the level with
	CollectibleSpawnSubsystem->InitializeCollectibleTables(
		CollectibleDropTable, RareCollectibleDropTable);
	
	// Spawn collectibles
	CollectibleSpawnSubsystem->SpawnCollectibles();
}
