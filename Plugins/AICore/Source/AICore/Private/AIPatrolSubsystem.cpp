// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPatrolSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "PatrolPoint.h"

void UAIPatrolSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Emptying on map loads
	AllPatrolPoints.Empty();
}

void UAIPatrolSubsystem::RegisterPatrolPoint(APatrolPoint* Point)
{
	// Make sure the point is valid and we havent already added it
	if (IsValid(Point) && !AllPatrolPoints.Contains(Point))
	{
		AllPatrolPoints.Add(Point);
	}
}

void UAIPatrolSubsystem::Deinitialize()
{
	// Clean up when the level ends
	AllPatrolPoints.Empty();
    
	Super::Deinitialize();
}
