// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_Zoom.h"
#include "CameraModifier_Zoom.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemComponent.h"


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
			// Remove old zoom modifier if exists
			if (UCameraModifier* LingeringModifier = PC->PlayerCameraManager->FindCameraModifierByClass(ZoomModifierClass))
			{
				PC->PlayerCameraManager->RemoveCameraModifier(LingeringModifier);
			}

			// Create zoom modifier
			UCameraModifier* CameraModifier = PC->PlayerCameraManager->AddNewCameraModifier(ZoomModifierClass);

			// Zoom modifier has default variable values already, but we can modify them
			if (UCameraModifier_Zoom* ZoomModifier = Cast<UCameraModifier_Zoom>(CameraModifier))
			{
				// Can modify variables within this GAs BP
				ZoomModifier->TargetFOV = TargetFOV;
				ZoomModifier->ZoomSpeed = ZoomSpeed;

				// Saving the reference so we can delete it later
				ActiveZoomModifier = ZoomModifier;
			}
		}
	}

	// Applying zooming GE. Could bind the actual zoom effect to GE. Might be unnecessary.
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ZoomingEffectClass, 1.0f);

	if (SpecHandle.IsValid())
	{
		ActiveEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}

	// Waiting for input release to stop zooming
	UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	if (InputReleaseTask)
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &UGA_Zoom::OnInputReleased);
		InputReleaseTask->ReadyForActivation();
	}
}

void UGA_Zoom::OnInputReleased(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Zoom::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActiveZoomModifier)
	{
		// ReverseModifier() reverses the zoom effect on the camera inside the CameraModifier.
		// Camera modifier removes itself when done reversing so we can forget about it
		ActiveZoomModifier->ReverseModifier();
		ActiveZoomModifier = nullptr;
	}

	// Cleaning up zoom GE
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