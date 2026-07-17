// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeTask_SetMoveGoal.generated.h"

class ACoreAIController;

USTRUCT()
struct FStateTreeTask_SetMoveGoalInstanceData
{
	GENERATED_BODY()

	// The generic input vector we want to apply to our movement goal.
	// Categorizing as "Input" automatically opens it up for bindings in the editor
	UPROPERTY(EditAnywhere, Category = "Input")
	FVector GoalValue = FVector::ZeroVector;
};

USTRUCT(meta = (DisplayName = "Set Move Goal", Category = "Movement"))
struct AICORE_API FStateTreeTask_SetMoveGoal : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeTask_SetMoveGoalInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	FStateTreeTask_SetMoveGoal();

	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

protected:
	TStateTreeExternalDataHandle<ACoreAIController> AIControllerHandle;
};