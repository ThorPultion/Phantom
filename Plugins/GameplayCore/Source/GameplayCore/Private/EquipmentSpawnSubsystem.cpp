// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentSpawnSubsystem.h"
#include "Actors/PickupEquipment.h"
#include "Data/ItemDropTable.h"
#include "Actors/EquipmentSpawnPoint.h"

void UEquipmentSpawnSubsystem::RegisterEquipmentSpawnPoint(AEquipmentSpawnPoint* SpawnPoint)
{
	SpawnPoints.Add(SpawnPoint);
}

void UEquipmentSpawnSubsystem::InitializeEquipmentTable(UItemDropTable* InDropTable)
{
	DropTable = InDropTable;
}

void UEquipmentSpawnSubsystem::SpawnEquipments()
{
	// The Subsystem shouldnt spawn things on clients
	if (GetWorld()->GetNetMode() == NM_Client) return;

	// Normal Spawns
	if (DropTable && !DropTable->ItemDefinitions.IsEmpty())
	{
		for (const AEquipmentSpawnPoint* Point : SpawnPoints)
		{
			SpawnEquipment(Point, DropTable);
		}
	}
}

void UEquipmentSpawnSubsystem::SpawnEquipment(
	const AEquipmentSpawnPoint* SpawnPoint, UItemDropTable* InDropTable) const
{
	if (!IsValid(SpawnPoint) || !InDropTable) return;
	
	if (InDropTable->ItemDefinitions.IsEmpty() || !InDropTable->ItemClassToSpawn) return;
	
	const FTransform SpawnTransform = SpawnPoint->GetActorTransform();
	
	// Spawning collectible based on the class defined by drop table
	APickupEquipment* SpawnedCollectible = 
		GetWorld()->SpawnActorDeferred<APickupEquipment>(InDropTable->ItemClassToSpawn, SpawnTransform);
	
	const int RandomIndex = FMath::RandRange(0, InDropTable->ItemDefinitions.Num() - 1);
	// Changing the collectible to a random one from table
	SpawnedCollectible->ChangeItem(InDropTable->ItemDefinitions[RandomIndex]);
	
	SpawnedCollectible->FinishSpawning(SpawnTransform);
}
