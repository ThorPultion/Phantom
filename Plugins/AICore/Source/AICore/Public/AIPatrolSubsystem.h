// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIPatrolSubsystem.generated.h"

class APatrolPoint;

/**
 * 
 */
UCLASS()
class AICORE_API UAIPatrolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	// Called when the world is torn down
	virtual void Deinitialize() override;

	/** Get all patrol points from the level */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	const TArray<APatrolPoint*>& GetAllPatrolPoints() const { return AllPatrolPoints; }
	
	/** Add patrol point to subsystem */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	void RegisterPatrolPoint(APatrolPoint* Point);

private:
	/** All patrol points from the level cached */
	UPROPERTY(Transient)
	TArray<APatrolPoint*> AllPatrolPoints;
};
