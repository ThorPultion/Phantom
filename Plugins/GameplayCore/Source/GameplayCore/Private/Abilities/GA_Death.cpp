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

	// Standard GAS safety check
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return;
	}

	FGameplayCueParameters CueParams;
	if (TriggerEventData)
	{
		CueParams.RawMagnitude = TriggerEventData->EventMagnitude;
		CueParams.EffectContext = TriggerEventData->ContextHandle;

		CueParams.Instigator = TriggerEventData->ContextHandle.GetInstigator();
		CueParams.EffectCauser = TriggerEventData->ContextHandle.GetEffectCauser();
	}

	// Fire the Cue to all clients
	ActorInfo->AbilitySystemComponent->ExecuteGameplayCue(GASCoreTags::GameplayCue_Death_Ragdoll, CueParams);

	// 3. Do not end the ability, so the character retains the "State.Dead" tag

	// --- 4. DO NOT END THE ABILITY ---
	// Notice there is no EndAbility() call down here. 
	// We leave the ability permanently active so the Character retains the "State.Dead" tag 
	// until they are respawned or destroyed.
}