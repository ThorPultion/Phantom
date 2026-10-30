// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoSpawnSubsystem.h"
#include "Actors/AmmoSpawnPoint.h"
#include "Actors/PickupAmmo.h"
#include "Data/ItemDropTable.h"

void UAmmoSpawnSubsystem::RegisterAmmoSpawnPoint(AAmmoSpawnPoint* SpawnPoint)
{
	SpawnPoints.Add(SpawnPoint);
}

void UAmmoSpawnSubsystem::InitializeAmmoTable(UItemDropTable* InDropTable)
{
	DropTable = InDropTable;
}

void UAmmoSpawnSubsystem::SpawnAmmos()
{
	// The Subsystem shouldnt spawn things on clients
	if (GetWorld()->GetNetMode() == NM_Client) return;

	// Spawns
	if (DropTable && !DropTable->ItemDefinitions.IsEmpty())
	{
		for (const AAmmoSpawnPoint* Point : SpawnPoints)
		{
			SpawnAmmo(Point, DropTable);
		}
	}
}

void UAmmoSpawnSubsystem::SpawnAmmo(const AAmmoSpawnPoint* SpawnPoint, UItemDropTable* InDropTable) const
{
	if (!IsValid(SpawnPoint) || !InDropTable) return;
	
	if (InDropTable->ItemDefinitions.IsEmpty() || !InDropTable->ItemClassToSpawn) return;
	
	const FTransform SpawnTransform = SpawnPoint->GetActorTransform();
	
	// Spawning collectible based on the class defined by drop table
	APickupAmmo* SpawnedAmmo = 
		GetWorld()->SpawnActorDeferred<APickupAmmo>(InDropTable->ItemClassToSpawn, SpawnTransform);
	
	const int RandomIndex = FMath::RandRange(0, InDropTable->ItemDefinitions.Num() - 1);
	// Changing the collectible to a random one from table
	SpawnedAmmo->ChangeItem(InDropTable->ItemDefinitions[RandomIndex]);
	
	SpawnedAmmo->FinishSpawning(SpawnTransform);
}
