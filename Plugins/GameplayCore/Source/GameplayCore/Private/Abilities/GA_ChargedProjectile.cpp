// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GA_ChargedProjectile.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "EquipmentComponent.h"


void UGA_ChargedProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PlayAbilityMontage(ChargeMontage);

	// Applying "Drawing" GE.
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ChargingEffectClass, 1.0f);

	if (SpecHandle.IsValid())
	{
		ActiveEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}

	// Waiting for input to release to fire bow
	UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	if (InputReleaseTask)
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &UGA_ChargedProjectile::OnInputReleased);
		InputReleaseTask->ReadyForActivation();
	}
}

void UGA_ChargedProjectile::OnInputReleased(float TimeHeld)
{
	// Minimal draw time
	if (TimeHeld >= MinChargeTime && ReleaseMontage)
	{
		PlayAbilityMontageAndWaitForEvent(ReleaseMontage, SpawnEventTag);
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UGA_ChargedProjectile::OnMontageEventReceived_Implementation(FGameplayEventData Payload)
{
	// CRUCIAL DECISION FOR MULTIPLAYER:
	// Do we care about client predicting the projectile?
	// 
	// A fake local projectile and hiding the server projectile for clients could be sufficient.
	// If projectiles are relatively small and predictable, it would be mostly accurate.
	// 
	// If movement speed of shooter or target is high, it may cause more frustrating inaccuracies.
	// If projectile speed is low, that could amplify the inaccuracies.

	if (!GetOwningActorFromActorInfo()->HasAuthority()) return;

	// Getting player
	AActor* Avatar = GetAvatarActorFromActorInfo();

	// Finding EquipmentComponent.
	// We could technically cache this in OnAvatarSet to avoid findcomponent time cost,
	// but with the current amount of actor components finding is
	// not expensive, and other problems could occur with caching.
	if (UEquipmentComponent* EquipComp = Avatar->FindComponentByClass<UEquipmentComponent>())
	{
		TSubclassOf<AActor> ClassToSpawn = EquipComp->GetCurrentProjectileClass();

		if (ClassToSpawn)
		{
			// TODO: Spawning logic
			FTransform SpawnTransform = Avatar->GetActorTransform();
			GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTransform);
		}
	}
	// We dont call EndAbility here because the wrapper handles it via 
	// the Montage Completed delegate, but we could in order to stop it early
}


void UGA_ChargedProjectile::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Cleaning up charging GE
	if (ActiveEffectHandle.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(ActiveEffectHandle);
		ActiveEffectHandle.Invalidate();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}