// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_Zoom.h"
#include "CameraModifier_Zoom.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemComponent.h"

UGA_Zoom::UGA_Zoom() { InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; }

void UGA_Zoom::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (APlayerController* PC = Cast<APlayerController>(GetAvatarActorFromActorInfo()->GetInstigatorController()))
	{
		if (PC->PlayerCameraManager && ZoomModifierClass)
		{
			// Find any old, zooming-out modifiers and execute them so they dont fight us
			if (UCameraModifier* LingeringModifier = PC->PlayerCameraManager->FindCameraModifierByClass(ZoomModifierClass))
			{
				PC->PlayerCameraManager->RemoveCameraModifier(LingeringModifier);
			}

			// 1. Create the modifier
			UCameraModifier* CreatedModifier = PC->PlayerCameraManager->AddNewCameraModifier(ZoomModifierClass);

			// 2. Cast it to our specific class so we can access our custom variables
			if (UCameraModifier_Zoom* ZoomMod = Cast<UCameraModifier_Zoom>(CreatedModifier))
			{
				// 3. Inject the variable! 
				ZoomMod->TargetFOV = 45.0f; // (You could even make THIS a variable in GA_Zoom.h)
				ZoomMod->ZoomSpeed = 12.0f;

				// 4. Save the reference so we can delete it later
				ActiveZoomModifier = ZoomMod;
			}
		}
	}

	// Applying zooming GE. Might be useful later. Could bind the actual zoom effect to GE. Might be unnecessary.
	// Zoom GE can say lower movement speed attribute

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ZoomingEffectClass, 1.0f);

	if (SpecHandle.IsValid())
	{
		ActiveEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}

	UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	if (InputReleaseTask)
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &UGA_Zoom::OnInputReleased);
		InputReleaseTask->ReadyForActivation();
	}
}

void UGA_Zoom::OnInputReleased(float TimeHeld)
{
	// Simply ending the ability will now trigger our EndAbility override below
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Zoom::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActiveZoomModifier)
	{
		ActiveZoomModifier->ReverseModifier();
		ActiveZoomModifier = nullptr; // We let it go, it will handle its own destruction
	}

	// 2. Cleanup: Remove the effect if it's still active
	if (ActiveEffectHandle.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
		}
		ActiveEffectHandle.Invalidate();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}