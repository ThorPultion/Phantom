// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_SetFocus.h"
#include "CoreAIController.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

FStateTreeTask_SetFocus::FStateTreeTask_SetFocus()
{
	bShouldCallTick = false;
}

bool FStateTreeTask_SetFocus::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(AIControllerHandle);
	return true;
}

EStateTreeRunStatus FStateTreeTask_SetFocus::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	ACoreAIController* AIController = Context.GetExternalDataPtr(AIControllerHandle);

	if (!IsValid(AIController) || !IsValid(InstanceData.TargetActor))
	{
		return EStateTreeRunStatus::Failed;
	}

	// AIController now continuously tracks actor
	AIController->SetFocus(InstanceData.TargetActor, EAIFocusPriority::Gameplay);

	// Returning running so this task stays active in the state alongside other tasks such as MoveTo
	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_SetFocus::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	ACoreAIController* AIController = Context.GetExternalDataPtr(AIControllerHandle);

	if (IsValid(AIController))
	{
		// The moment the state changes, we nuke the focus to avoid lingering rotation
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}