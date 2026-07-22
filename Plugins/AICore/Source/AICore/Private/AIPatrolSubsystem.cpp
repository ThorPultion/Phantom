// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPatrolSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "PatrolPoint.h"

void UAIPatrolSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Emptying for safety in editor
	AllPatrolPoints.Empty();

	// Finding all patrol points in the level
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(&InWorld, APatrolPoint::StaticClass(), FoundActors);

	// Reserving memory to avoid reallocations during the loop (Performance best practice)
	AllPatrolPoints.Reserve(FoundActors.Num());

	// Casting them and adding them to our typed array
	for (AActor* Actor : FoundActors)
	{
		if (APatrolPoint* Point = Cast<APatrolPoint>(Actor))
		{
			AllPatrolPoints.Add(Point);
		}
	}
}

void UAIPatrolSubsystem::Deinitialize()
{
	// Clean up when the level ends
	AllPatrolPoints.Empty();
    
	Super::Deinitialize();
}
