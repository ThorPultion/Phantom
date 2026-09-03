// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolPoint.h"

#include "AIPatrolSubsystem.h"
#include "Components/ArrowComponent.h"

// Sets default values
APatrolPoint::APatrolPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// The object needs a root for transform data.
	// Without this, in packaged builds the object will go to world origin.
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	// Editor only indicators for level placements
#if WITH_EDITORONLY_DATA
	EditorIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EditorIndicator"));
	EditorIndicator->SetupAttachment(RootComponent);
	EditorIndicator->SetHiddenInGame(true);
	
	FacingDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("FacingDirection"));
	FacingDirection->SetupAttachment(RootComponent);
    
	// Making arrow more visible
	FacingDirection->ArrowColor = FColor::Orange;
	FacingDirection->ArrowSize = 1.5f;
#endif
}

void APatrolPoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Registering patrol point to subsystem
	if (const UWorld* World = GetWorld())
	{
		if (UAIPatrolSubsystem* PatrolSubsystem = World->GetSubsystem<UAIPatrolSubsystem>())
		{
			PatrolSubsystem->RegisterPatrolPoint(this);
		}
	}
}

void APatrolPoint::Claim(AActor* Claimer)
{
	if (!bIsClaimed)
	{
		bIsClaimed = true;
		CurrentClaimer = Claimer;
	}
}

void APatrolPoint::Release()
{
	bIsClaimed = false;
	CurrentClaimer = nullptr;
}
