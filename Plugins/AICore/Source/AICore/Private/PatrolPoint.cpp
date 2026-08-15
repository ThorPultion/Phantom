// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolPoint.h"

#include "AIPatrolSubsystem.h"
#include "Components/ArrowComponent.h"

// Sets default values
APatrolPoint::APatrolPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	
	EditorIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EditorIndicator"));
	SetRootComponent(EditorIndicator);
	EditorIndicator->SetHiddenInGame(true);
	
	FacingDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("FacingDirection"));
	FacingDirection->SetupAttachment(RootComponent);
    
	// Making arrow more visible
	FacingDirection->ArrowColor = FColor::Orange;
	FacingDirection->ArrowSize = 1.5f;
}

void APatrolPoint::BeginPlay()
{
	Super::BeginPlay();

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

