// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_Death.h"
#include "GASCoreTags.h"
#include "AbilitySystemComponent.h"

UGA_Death::UGA_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthority(&ActivationInfo))
	{
		FGameplayCueParameters CueParams;
		if (TriggerEventData)
		{
			CueParams.RawMagnitude = TriggerEventData->EventMagnitude;
			CueParams.EffectContext = TriggerEventData->ContextHandle;

			CueParams.Instigator = TriggerEventData->ContextHandle.GetInstigator();
			CueParams.EffectCauser = TriggerEventData->ContextHandle.GetEffectCauser();
		}

		// Ragdolling the character with a Cue so its properly replicated
		ActorInfo->AbilitySystemComponent->ExecuteGameplayCue(GASCoreTags::GameplayCue_Death_Ragdoll, CueParams);
	}

	// WE ARE NOT CALLING ENDABILITY! The Character retains the "State.Dead" tag 
	// until respawned or destroyed.
}