// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_ReleasePatrolPoint.h"
#include "StateTreeExecutionContext.h"
#include "PatrolPoint.h"

/* FStateTreeTask_ReleasePatrolPoint::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// Validate and release the point if it is currently claimed
	if (InstanceData.PatrolPointToRelease && InstanceData.PatrolPointToRelease->IsClaimed())
	{
		InstanceData.PatrolPointToRelease->Release();
	}
	
	// Immediately moving on
	return EStateTreeRunStatus::Succeeded;
}*/

void FStateTreeTask_ReleasePatrolPoint::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// If we are holding a valid, claimed patrol point when exiting this state, release it!
	if (InstanceData.PatrolPointToRelease && InstanceData.PatrolPointToRelease->IsClaimed())
	{
		InstanceData.PatrolPointToRelease->Release();
		InstanceData.PatrolPointToRelease = nullptr;
	}
}