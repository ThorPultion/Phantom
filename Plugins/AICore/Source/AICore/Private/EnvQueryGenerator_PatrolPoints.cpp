// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryGenerator_PatrolPoints.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "AIPatrolSubsystem.h"
#include "PatrolPoint.h"

UEnvQueryGenerator_PatrolPoints::UEnvQueryGenerator_PatrolPoints(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	// Telling EQS this generator outputs Actors, not just raw FVectors (Locations)
	ItemType = UEnvQueryItemType_Actor::StaticClass();
}

void UEnvQueryGenerator_PatrolPoints::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	const UWorld* World = QueryInstance.World;
	if (!IsValid(World)) return;

	const UAIPatrolSubsystem* PatrolSubsystem = World->GetSubsystem<UAIPatrolSubsystem>();
	if (!IsValid(PatrolSubsystem)) return;
	
	TArray<AActor*> ValidPoints;
	
	for (APatrolPoint* Point : PatrolSubsystem->GetAllPatrolPoints())
	{
		// Only add points that are valid and NOT currently claimed by another AI
		if (IsValid(Point) && !Point->IsClaimed())
		{
			ValidPoints.Add(Point);
		}
	}

	// Feeding the filtered patrol points to EQS
	QueryInstance.AddItemData<UEnvQueryItemType_Actor>(ValidPoints);
}

FText UEnvQueryGenerator_PatrolPoints::GetDescriptionTitle() const
{
	return FText::FromString(TEXT("Unclaimed Patrol Points"));
}

FText UEnvQueryGenerator_PatrolPoints::GetDescriptionDetails() const
{
	return FText::FromString(TEXT("Grabs all available patrol points from the AIPatrolSubsystem"));
}