// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeTask_TurnToLocation.generated.h"

class ACoreAIController;

USTRUCT()
struct FStateTreeTask_TurnToLocationInstanceData
{
	GENERATED_BODY()

	// The location we want to look at
	UPROPERTY(EditAnywhere, Category = "Input")
	FVector TargetLocation = FVector::ZeroVector;

	// How close to the exact angle is close enough to finish the task
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AngleTolerance = 5.0f;
};

USTRUCT(meta = (DisplayName = "Turn To Location", Category = "Rotation"))
struct AICORE_API FStateTreeTask_TurnToLocation : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeTask_TurnToLocationInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	FStateTreeTask_TurnToLocation();

	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
	TStateTreeExternalDataHandle<ACoreAIController> AIControllerHandle;
};