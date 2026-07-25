// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeLineOfSightCondition.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"

bool FStateTreeLineOfSightCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	// Taking the inputs
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	// No LoS if targets dont exist
	if (!InstanceData.AIController || !InstanceData.TargetActor)
	{
		return false;
	}
	
	if (const UAIPerceptionComponent* PerceptionComp = InstanceData.AIController->GetAIPerceptionComponent())
	{
		// Checking through perception component if there is an active sight stimulus
		const FAISenseID SightSenseID = UAISense::GetSenseID<UAISense_Sight>();
		return PerceptionComp->HasActiveStimulus(*InstanceData.TargetActor, SightSenseID);
	}

	return false;
}
