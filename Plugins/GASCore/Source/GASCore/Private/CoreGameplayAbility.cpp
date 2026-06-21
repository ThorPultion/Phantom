// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGameplayAbility.h"
#include "AnimNotify_GameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UCoreGameplayAbility::UCoreGameplayAbility()
{
	// Set safe defaults so designers dont break multiplayer
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UCoreGameplayAbility::PlayAbilityMontage(UAnimMontage* MontageToPlay)
{
	if (!MontageToPlay) return;

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, MontageToPlay, 1.0f, NAME_None, false);

	// Bind standard cleanup events
	MontageTask->OnBlendOut.AddDynamic(this, &UCoreGameplayAbility::OnMontageCompleted);
	MontageTask->OnCompleted.AddDynamic(this, &UCoreGameplayAbility::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UCoreGameplayAbility::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UCoreGameplayAbility::OnMontageCancelled);

	MontageTask->ReadyForActivation();
}

void UCoreGameplayAbility::PlayAbilityMontageAndWaitForEvent(UAnimMontage* MontageToPlay, FGameplayTag EventTag)
{
	if (!MontageToPlay) return;

	PlayAbilityMontage(MontageToPlay);

	// Wait for AnimNotify_GameplayEvent
	// Parameters: Owning Ability, Tag to listen for, Optional Target (nullptr means listen to self), OnlyTriggerOnce, OnlyMatchExact
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, EventTag, nullptr, false, false);

	if (EventTask)
	{
		// Bind for when AnimNotify is received
		EventTask->EventReceived.AddDynamic(this, &UCoreGameplayAbility::OnMontageEventReceived);
		
		// Activate task so it starts listening
		EventTask->ReadyForActivation();
	}
}

void UCoreGameplayAbility::OnMontageCompleted()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCoreGameplayAbility::OnMontageCancelled()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCoreGameplayAbility::OnMontageEventReceived_Implementation(FGameplayEventData Payload)
{
	// Child classes will override this for functionality that occurs when an AnimNotify is received
}
