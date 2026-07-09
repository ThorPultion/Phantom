// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAIStateEvaluator.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"

void FCoreAIStateEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	// Grab the memory block for this specific AI
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// Ensure we have a valid actor
	if (AActor* TargetActor = InstanceData.TargetActor)
	{
		// Safely grab the Ability System Component using Epics global function
		// This works whether the ASC is on the Pawn (like objects and AI) or the PlayerState
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			// Dump all the currently active tags on the character into our output variable
			ASC->GetOwnedGameplayTags(InstanceData.ActiveStateTags);
		}
		else
		{
			// Clear it out if no ASC is found to prevent stale data
			InstanceData.ActiveStateTags.Reset();
		}
	}
}