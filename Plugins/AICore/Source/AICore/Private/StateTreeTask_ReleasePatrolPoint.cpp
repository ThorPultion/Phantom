// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_ReleasePatrolPoint.h"
#include "StateTreeExecutionContext.h"
#include "PatrolPoint.h"

EStateTreeRunStatus FStateTreeTask_ReleasePatrolPoint::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// If we are holding a valid, claimed patrol point when exiting this state, we release it
	if (InstanceData.PatrolPointToRelease && InstanceData.PatrolPointToRelease->IsClaimed())
	{
		InstanceData.PatrolPointToRelease->Release();
	}
	
	InstanceData.OutPatrolPoint = nullptr;
	
	return EStateTreeRunStatus::Succeeded;
}