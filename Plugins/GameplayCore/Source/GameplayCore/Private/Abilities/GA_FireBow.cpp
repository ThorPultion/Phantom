// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GA_FireBow.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"

UGA_FireBow::UGA_FireBow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_FireBow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PlayAbilityMontage(DrawMontage);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	if (ASC && DrawingEffectClass)
	{
		// 1. Create a Context (needed for the effect to know who applied it)
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		// 2. Create the Spec (The "Blueprint" of the effect)
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DrawingEffectClass, 1.0f, EffectContext);

		// 3. Apply the Spec (The actual application of the effect)
		if (SpecHandle.IsValid())
		{
			// *Note: Using Data.Get() extracts the actual spec object from the handle
			ActiveEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	if (InputReleaseTask)
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &UGA_FireBow::OnInputReleased);
		InputReleaseTask->ReadyForActivation();
	}
}

void UGA_FireBow::OnInputReleased(float TimeHeld)
{
	if (TimeHeld >= 0.2f) // Minimal draw time
	{
		PlayAbilityMontageAndWaitForEvent(FireMontage, FireEventTag);
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UGA_FireBow::OnMontageEventReceived_Implementation(FGameplayEventData Payload)
{
	if (GetOwningActorFromActorInfo()->HasAuthority() && ArrowClass)
	{
		// Note: Later, you can replace 'ArrowClass' with a call to your 
		// EquipmentComponent or Inventory to handle dynamic arrow swapping.

		FTransform SpawnTransform = GetAvatarActorFromActorInfo()->GetActorTransform();
		GetWorld()->SpawnActor<AActor>(ArrowClass, SpawnTransform);
	}

	// We don't call EndAbility here because the wrapper handles it via 
	// the Montage Completed delegate, but you can if you need to stop early.
}

void UGA_FireBow::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 2. Cleanup: Remove the effect if it's still active
	if (ActiveEffectHandle.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(ActiveEffectHandle);
		ActiveEffectHandle.Invalidate();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}