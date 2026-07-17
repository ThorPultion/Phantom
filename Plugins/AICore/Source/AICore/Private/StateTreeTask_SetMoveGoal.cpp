// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_SetMoveGoal.h"
#include "CoreAIController.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

FStateTreeTask_SetMoveGoal::FStateTreeTask_SetMoveGoal()
{
	bShouldCallTick = false;
}

bool FStateTreeTask_SetMoveGoal::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(AIControllerHandle);
	return true;
}

EStateTreeRunStatus FStateTreeTask_SetMoveGoal::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	ACoreAIController* AIController = Context.GetExternalDataPtr(AIControllerHandle);

	if (IsValid(AIController))
	{
		return EStateTreeRunStatus::Failed;
	}
	
	AIController->CurrentMovementGoal = InstanceData.GoalValue;

	return EStateTreeRunStatus::Succeeded;
}
