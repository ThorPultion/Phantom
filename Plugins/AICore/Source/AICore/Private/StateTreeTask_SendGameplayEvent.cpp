// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_SendGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FStateTreeTask_SendGameplayEvent::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.TargetActor && InstanceData.EventTag.IsValid())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor))
		{
			FGameplayEventData Payload;
			Payload.Instigator = InstanceData.TargetActor;
			Payload.EventTag = InstanceData.EventTag;
			if (InstanceData.PayloadTarget)
			{
				Payload.Target = InstanceData.PayloadTarget;
			}

			ASC->HandleGameplayEvent(InstanceData.EventTag, &Payload);

			// If we want to wait, we tell the StateTree this task is still Running
			if (InstanceData.bWaitForCompletion && InstanceData.InProgressTag.IsValid())
			{
				return EStateTreeRunStatus::Running;
			}

			return EStateTreeRunStatus::Succeeded;
		}
	}
	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FStateTreeTask_SendGameplayEvent::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.TargetActor))
	{
		// As long as the AI has the matching tag, we pause the task and statetree
		if (ASC->HasMatchingGameplayTag(InstanceData.InProgressTag))
		{
			return EStateTreeRunStatus::Running;
		}
	}

	// The tag is gone which should signify that the ability is over so we can safely end the task
	return EStateTreeRunStatus::Succeeded;
}