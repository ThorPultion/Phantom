// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_Knockback.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/PrimitiveComponent.h"

UGA_Knockback::UGA_Knockback()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGA_Knockback::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!AvatarActor || !TriggerEventData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Hit and magnitude data passed here through payload
	float KnockbackMagnitude = TriggerEventData->EventMagnitude;
	FVector ImpactNormal = FVector::ZeroVector;
	FVector ImpactLocation = AvatarActor->GetActorLocation();
	FVector PushDirection = FVector::ZeroVector;

	// Using hit data for location and direction
	if (const FHitResult* HitResult = TriggerEventData->ContextHandle.GetHitResult())
	{
		ImpactNormal = HitResult->ImpactNormal;
		ImpactLocation = HitResult->ImpactPoint;
		PushDirection = (HitResult->TraceEnd - HitResult->TraceStart).GetSafeNormal();
	}
	else if (const AActor* Instigator = TriggerEventData->Instigator.Get())
	{
		ImpactNormal = (AvatarActor->GetActorLocation() - Instigator->GetActorLocation()).GetSafeNormal();
	}

	if (PushDirection.IsNearlyZero())
	{
		PushDirection = -ImpactNormal;
	}
	PushDirection.Z = FMath::Max(PushDirection.Z, ImpulseLiftModifier);
	PushDirection.Normalize();

	// If character, do root motion task. If not, do impulse
	if (UCharacterMovementComponent* CMC = AvatarActor->FindComponentByClass<UCharacterMovementComponent>())
	{
		UAbilityTask_ApplyRootMotionConstantForce* KnockbackTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
			this,
			TEXT("KnockbackTask"),
			PushDirection,
			KnockbackMagnitude,
			0.1f, // Duration, we want practically just an impulse
			true,
			nullptr,
			ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
			FVector::ZeroVector,
			0.f,
			false
		);

		if (KnockbackTask)
		{
			KnockbackTask->OnFinish.AddDynamic(this, &UGA_Knockback::OnKnockbackFinished);
			KnockbackTask->ReadyForActivation();
		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
	}
	else
	{
		if (HasAuthority(&ActivationInfo))
		{
			if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(AvatarActor->GetRootComponent()))
			{
				if (PrimitiveComp->IsSimulatingPhysics())
				{
					// Note: Root motion magnitude is in cm/s. Physics impulse takes into account mass.
					// Multiplier is required to reach similar knockback forces
					FVector FinalImpulse = PushDirection * (KnockbackMagnitude * NonCharacterImpulseMultiplier);

					PrimitiveComp->AddImpulseAtLocation(FinalImpulse, ImpactLocation);
				}
			}
		}

		// Physics impulses are instant, so we shut the ability down immediately
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UGA_Knockback::OnKnockbackFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}