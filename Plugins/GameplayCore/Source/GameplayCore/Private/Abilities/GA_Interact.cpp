// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_Interact.h"
#include "Interactable.h"


void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

	// Taking target from payload and making sure its interactable
	if (TriggerEventData && TriggerEventData->Target)
	{
		if (TriggerEventData->Target->Implements<UInteractable>())
		{
			// Interface function execute requires UObject* so we have to const_cast from payload data
			// to get a non read only pointer.
			IInteractable::Execute_Interact(const_cast<AActor*>(TriggerEventData->Target.Get()), ActorInfo->AvatarActor.Get());
		}
	}

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}