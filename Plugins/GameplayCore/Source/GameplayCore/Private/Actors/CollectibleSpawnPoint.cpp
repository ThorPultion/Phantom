// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CollectibleSpawnPoint.h"
#include "CollectibleSpawnSubsystem.h"
#include "Components/ArrowComponent.h"

// Sets default values
ACollectibleSpawnPoint::ACollectibleSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Editor only indicators for level placements
#if WITH_EDITORONLY_DATA
	EditorIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EditorIndicator"));
	SetRootComponent(EditorIndicator);
	EditorIndicator->SetHiddenInGame(true);
	
	FacingDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("FacingDirection"));
	FacingDirection->SetupAttachment(RootComponent);
    
	// Making arrow more visible
	FacingDirection->ArrowColor = FColor::Orange;
	FacingDirection->ArrowSize = 1.5f;
#endif
}

void ACollectibleSpawnPoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (!HasAuthority()) return;
	
	if (const UWorld* World = GetWorld())
	{
		// Subsystem handles spawning the collectible
		if (UCollectibleSpawnSubsystem* CollectibleSpawnSubsystem = World->GetSubsystem<UCollectibleSpawnSubsystem>())
		{
			CollectibleSpawnSubsystem->RegisterCollectibleSpawnPoint(this, bIsRare);
		}
	}
}
