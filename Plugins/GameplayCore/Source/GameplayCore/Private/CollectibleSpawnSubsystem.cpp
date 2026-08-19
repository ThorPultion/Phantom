// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectibleSpawnSubsystem.h"
#include "Data/CollectibleDefinition.h"
#include "Actors/PickupCollectible.h"
#include "Data/CollectibleDropTable.h"
#include "Actors/CollectibleSpawnPoint.h"

void UCollectibleSpawnSubsystem::RegisterCollectibleSpawnPoint(ACollectibleSpawnPoint* SpawnPoint, const bool bIsRare)
{
	if (bIsRare)
	{
		RareSpawnPoints.Add(SpawnPoint);
	}
	else
	{
		SpawnPoints.Add(SpawnPoint);
	}
}

void UCollectibleSpawnSubsystem::InitializeCollectibleTables(UCollectibleDropTable* InDropTable,
	UCollectibleDropTable* InRareDropTable)
{
	DropTable = InDropTable;
	RareDropTable = InRareDropTable;
}

void UCollectibleSpawnSubsystem::SpawnCollectibles()
{
	// The Subsystem shouldnt spawn things on clients
	if (GetWorld()->GetNetMode() == NM_Client) return;

	// Normal Spawns
	if (DropTable && !DropTable->CollectibleDefinitions.IsEmpty())
	{
		for (const ACollectibleSpawnPoint* Point : SpawnPoints)
		{
			SpawnCollectible(Point, DropTable);
		}
	}

	// Rare Spawns
	if (RareDropTable && !RareDropTable->CollectibleDefinitions.IsEmpty())
	{
		for (const ACollectibleSpawnPoint* Point : RareSpawnPoints)
		{
			SpawnCollectible(Point, RareDropTable);
		}
	}
}

void UCollectibleSpawnSubsystem::SpawnCollectible(
	const ACollectibleSpawnPoint* SpawnPoint, UCollectibleDropTable* InDropTable) const
{
	if (!IsValid(SpawnPoint) || !InDropTable) return;
	
	if (InDropTable->CollectibleDefinitions.IsEmpty() || !InDropTable->CollectibleClassToSpawn) return;
	
	const FTransform SpawnTransform = SpawnPoint->GetActorTransform();
	
	// Spawning collectible based on the class defined by drop table
	APickupCollectible* SpawnedCollectible = 
		GetWorld()->SpawnActorDeferred<APickupCollectible>(InDropTable->CollectibleClassToSpawn, SpawnTransform);
	
	const int RandomIndex = FMath::RandRange(0, InDropTable->CollectibleDefinitions.Num() - 1);
	// Changing the collectible to a random one from table
	SpawnedCollectible->ChangeItem(InDropTable->CollectibleDefinitions[RandomIndex]);
	
	SpawnedCollectible->FinishSpawning(SpawnTransform);
}
