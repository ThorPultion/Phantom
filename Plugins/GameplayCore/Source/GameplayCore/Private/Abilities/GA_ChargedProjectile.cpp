// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GA_ChargedProjectile.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "EquipmentComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EquipmentBase.h"
#include "ProjectileProvider.h"


void UGA_ChargedProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Officially commits the ability (checks cost, cooldowns, etc.)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

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
		// This bool prevents montage cancellation due to "chaining" a montage after another
		bIsChainingMontages = true;
		PlayAbilityMontageAndWaitForEvent(ReleaseMontage, SpawnEventTag);
		bIsChainingMontages = false;
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
	if (!IsValid(Avatar)) return;

	// Finding EquipmentComponent.
	// We could technically cache this in OnAvatarSet to avoid findcomponent time cost,
	// but with the current amount of actor components finding is
	// not expensive, and other problems could occur with caching.
	UEquipmentComponent* EquipComp = Avatar->FindComponentByClass<UEquipmentComponent>();
	if (!EquipComp) return;

	AEquipmentBase* EquippedWeapon = EquipComp->GetCurrentItem();
	if (!IsValid(EquippedWeapon)) return;
	if (!EquippedWeapon->Implements<UProjectileProvider>()) return;

	TSubclassOf<AActor> ProjectileToSpawn = IProjectileProvider::Execute_GetCurrentProjectileClass(EquippedWeapon);
	if (!ProjectileToSpawn) return;

	FVector SpawnLocation = Avatar->GetActorLocation();
	FTransform SocketTransform = IProjectileProvider::Execute_GetProjectileSpawnTransform(EquippedWeapon);
	SpawnLocation = SocketTransform.GetLocation();

	APawn* AvatarPawn = Cast<APawn>(Avatar);

	FRotator SpawnRotation;

	if (AvatarPawn)
	{
		// If AI, use this rotation
		SpawnRotation = AvatarPawn->GetBaseAimRotation();

		if (APlayerController* PC = Cast<APlayerController>(AvatarPawn->GetController()))
		{
			// --- PLAYER LOGIC: Camera Line Trace ---
			FVector CameraLoc;
			FRotator CameraRot;

			// Extract location and rotation from camera
			PC->GetPlayerViewPoint(CameraLoc, CameraRot);

			// Trace way out into the distance (e.g., 10,000 units = 100 meters)
			FVector TraceEnd = CameraLoc + (CameraRot.Vector() * 10000.0f);

			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(Avatar);

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				CameraLoc,
				TraceEnd,
				ECC_Visibility, // Could make a new trace channel for aiming?
				QueryParams
			);

			// If trace hit something, aim at the impact point. 
			// If trace hit nothing, aim at the end of the trace
			FVector TargetLocation = bHit ? HitResult.ImpactPoint : TraceEnd;

			// Angle the arrow from the bow socket to the target
			SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetLocation);
		}
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = AvatarPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(ProjectileToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

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