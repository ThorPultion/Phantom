// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "StateTreeTask_ReleasePatrolPoint.generated.h"

class APatrolPoint;

/** Instance data for releasing a patrol point. */
USTRUCT()
struct AICORE_API FStateTreeTask_ReleasePatrolPoint_InstanceData
{
	GENERATED_BODY()

	/** Input: The patrol point actor to release */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<APatrolPoint> PatrolPointToRelease = nullptr;
	
	/** Output: Used to overwrite the bound parameter back to null */
	UPROPERTY(EditAnywhere, Category = "Output")
	TObjectPtr<APatrolPoint> OutPatrolPoint = nullptr;
};

/** Releases a claimed patrol point so other AI can claim and use it. */
USTRUCT(meta = (DisplayName = "Release Patrol Point"), Category = "Patrol")
struct AICORE_API FStateTreeTask_ReleasePatrolPoint : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeTask_ReleasePatrolPoint_InstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};