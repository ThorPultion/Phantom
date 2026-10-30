// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectibleSpawnSubsystem.h"
#include "Data/CollectibleDefinition.h"
#include "Actors/PickupCollectible.h"
#include "Data/ItemDropTable.h"
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

void UCollectibleSpawnSubsystem::InitializeCollectibleTables(UItemDropTable* InDropTable,
	UItemDropTable* InRareDropTable)
{
	DropTable = InDropTable;
	RareDropTable = InRareDropTable;
}

void UCollectibleSpawnSubsystem::SpawnCollectibles()
{
	// The Subsystem shouldnt spawn things on clients
	if (GetWorld()->GetNetMode() == NM_Client) return;

	// Normal Spawns
	if (DropTable && !DropTable->ItemDefinitions.IsEmpty())
	{
		for (const ACollectibleSpawnPoint* Point : SpawnPoints)
		{
			SpawnCollectible(Point, DropTable);
		}
	}

	// Rare Spawns
	if (RareDropTable && !RareDropTable->ItemDefinitions.IsEmpty())
	{
		for (const ACollectibleSpawnPoint* Point : RareSpawnPoints)
		{
			SpawnCollectible(Point, RareDropTable);
		}
	}
}

void UCollectibleSpawnSubsystem::SpawnCollectible(
	const ACollectibleSpawnPoint* SpawnPoint, UItemDropTable* InDropTable) const
{
	if (!IsValid(SpawnPoint) || !InDropTable) return;
	
	if (InDropTable->ItemDefinitions.IsEmpty() || !InDropTable->ItemClassToSpawn) return;
	
	const FTransform SpawnTransform = SpawnPoint->GetActorTransform();
	
	// Spawning collectible based on the class defined by drop table
	APickupCollectible* SpawnedCollectible = 
		GetWorld()->SpawnActorDeferred<APickupCollectible>(InDropTable->ItemClassToSpawn, SpawnTransform);
	
	const int RandomIndex = FMath::RandRange(0, InDropTable->ItemDefinitions.Num() - 1);
	// Changing the collectible to a random one from table
	SpawnedCollectible->ChangeItem(InDropTable->ItemDefinitions[RandomIndex]);
	
	SpawnedCollectible->FinishSpawning(SpawnTransform);
}
