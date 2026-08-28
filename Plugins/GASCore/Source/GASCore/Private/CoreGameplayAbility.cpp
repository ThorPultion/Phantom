// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGameplayAbility.h"
#include "AnimNotify_GameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"

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
	// If we are intentionally chaining to a new montage, do not kill the ability
	if (bIsChainingMontages) return;

	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCoreGameplayAbility::OnMontageEventReceived_Implementation(FGameplayEventData Payload)
{
	// Child classes will override this for functionality that occurs when an AnimNotify is received
}

bool UCoreGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	// If we have no cost assigned, were free to activate
	const UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (!CostGE) return true;

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!IsValid(ASC)) return false;

	for (const FGameplayModifierInfo& ModInfo : CostGE->Modifiers)
	{
		if (ModInfo.ModifierMagnitude.GetMagnitudeCalculationType() != EGameplayEffectMagnitudeCalculation::SetByCaller)
		{
			continue;
		}

		const FGameplayTag DataTag = ModInfo.ModifierMagnitude.GetSetByCallerFloat().DataTag;
		const FScalableFloat* CostValue = CostData.Find(DataTag);

		// If the tags dont match, go next
		if (!CostValue) continue;

		const float Cost = CostValue->GetValueAtLevel(GetAbilityLevel());
		const float CurrentAttributeValue = ASC->GetNumericAttribute(ModInfo.Attribute);
          
		// Cannot afford attribute cost, prevent the commit
		if (CurrentAttributeValue < FMath::Abs(Cost)) return false;
	}

	return true;
}

void UCoreGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (!CostGE) return;
	
	// Building the spec for applying the cost
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), GetAbilityLevel());
	if (!SpecHandle.IsValid()) return;

	// Injecting the cost magnitudes to the cost GE
	for (const TPair<FGameplayTag, FScalableFloat>& CostPair : CostData)
	{
		const FGameplayTag& CostTag = CostPair.Key;
		const float CurrentCost = CostPair.Value.GetValueAtLevel(GetAbilityLevel());
              
		SpecHandle.Data->SetSetByCallerMagnitude(CostTag, CurrentCost);
	}

	(void)ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}