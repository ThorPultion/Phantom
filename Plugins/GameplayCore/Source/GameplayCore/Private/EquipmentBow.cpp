// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentBow.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "ArrowProjectile.h"

AEquipmentBow::AEquipmentBow()
{
	CurrentArrowIndex = 0;

	// Visual dummy arrow. 
	// In Blueprint, we will attach this to your FirstPerson/ThirdPerson bowstring socket
	FirstPersonLoadedArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstPersonLoadedArrowMesh"));
	ThirdPersonLoadedArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ThirdPersonLoadedArrowMesh"));
	FirstPersonLoadedArrowMesh->SetupAttachment(FirstPersonMesh);
	ThirdPersonLoadedArrowMesh->SetupAttachment(ThirdPersonMesh);

	FirstPersonLoadedArrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ThirdPersonLoadedArrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEquipmentBow::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEquipmentBow, CurrentArrowIndex);
}

TSubclassOf<AActor> AEquipmentBow::GetCurrentProjectileClass_Implementation() const
{
	if (AvailableArrowClasses.IsValidIndex(CurrentArrowIndex))
	{
		return AvailableArrowClasses[CurrentArrowIndex];
	}
	return nullptr;
}

FTransform AEquipmentBow::GetProjectileSpawnTransform_Implementation() const
{
	APawn* OwningPawn = Cast<APawn>(GetOwner());

	// Local player uses First Person Mesh
	if (OwningPawn && OwningPawn->IsLocallyControlled())
	{
		if (FirstPersonMesh)
		{
			return FirstPersonMesh->GetSocketTransform(ShootingPointSocket);
		}
	}

	// Server and other players use Third Person Mesh
	if (ThirdPersonMesh)
	{
		return ThirdPersonMesh->GetSocketTransform(ShootingPointSocket);
	}

	// Fallback
	return GetActorTransform();
}

void AEquipmentBow::CycleAmmo_Implementation(int32 Direction)
{
	// Strict Server Authority check
	if (!HasAuthority()) return;

	if (AvailableArrowClasses.IsEmpty()) return;

	// Add the direction (-1 or 1)
	int32 NewIndex = CurrentArrowIndex + Direction;

	// Wrap the index if we go out of bounds
	if (NewIndex < 0)
	{
		// Wrap to the end
		NewIndex = AvailableArrowClasses.Num() - 1;
	}
	else if (NewIndex >= AvailableArrowClasses.Num())
	{
		// Wrap to the start
		NewIndex = 0;
	}

	// Update the authoritative state (which triggers replication to clients)
	// and update the visuals locally if the server is also a playing host.
	CurrentArrowIndex = NewIndex;
	OnArrowVisualsChanged(CurrentArrowIndex);
}

void AEquipmentBow::SetAmmoIndex_Implementation(int32 SpecificIndex)
{
	// Strict Server Authority check
	if (!HasAuthority()) return;

	if (!AvailableArrowClasses.IsValidIndex(SpecificIndex)) return;

	// Dont do anything if we are already holding the same index
	if (CurrentArrowIndex == SpecificIndex) return;

	// Update the authoritative state (which triggers replication to clients)
	// and update the visuals locally if the server is also a playing host.
	CurrentArrowIndex = SpecificIndex;
	OnArrowVisualsChanged(CurrentArrowIndex);
}

void AEquipmentBow::OnRep_CurrentArrowIndex()
{
	// When the server tells simulated proxies about the new index, update the mesh
	OnArrowVisualsChanged(CurrentArrowIndex);
}