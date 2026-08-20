// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GA_ChargedProjectile.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "GASCoreTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Actor.h"
#include "Components/EquipmentComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Actors/EquipmentBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/ProjectileProvider.h"

UGA_ChargedProjectile::UGA_ChargedProjectile()
{
	FAbilityTriggerData TriggerData;
    
	// The tag to trigger this ability
	TriggerData.TriggerTag = GASCoreTags::Event_AI_Primary;
    
	// Source (matches "On Gameplay Event" in the BP dropdown)
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    
	// Adding it to the standard GAS triggers array
	AbilityTriggers.Add(TriggerData);
}

void UGA_ChargedProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Officially commits the ability (checks cost, cooldowns, etc.)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Applying "Drawing" GE.
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ChargingEffectClass, 1.0f);

	if (SpecHandle.IsValid())
	{
		ActiveEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}

	// --- PLAYER LOGIC ---
	// Waiting for physical input to release to fire projectile
	UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	if (InputReleaseTask)
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &UGA_ChargedProjectile::OnInputReleased);
		InputReleaseTask->ReadyForActivation();
	}
	
	// --- AI LOGIC ---
	// Waiting for Gameplay Event with AIs "release input" tag
	UAbilityTask_WaitGameplayEvent* AIInputReleaseEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, GASCoreTags::Event_AI_ReleasePrimary);
	if (AIInputReleaseEventTask)
	{
		AIInputReleaseEventTask->EventReceived.AddDynamic(this, &UGA_ChargedProjectile::OnAIReleaseEventReceived);
		AIInputReleaseEventTask->ReadyForActivation();
	}
}

void UGA_ChargedProjectile::OnInputReleased(const float TimeHeld)
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
	
	APawn* AvatarPawn = Cast<APawn>(Avatar);
	if (!IsValid(AvatarPawn)) return;

	// Finding EquipmentComponent.
	// We could technically cache this in OnAvatarSet to avoid findcomponent time cost,
	// but with the current amount of actor components finding is
	// not expensive, and other problems could occur with caching.
	UEquipmentComponent* EquipComp = Avatar->FindComponentByClass<UEquipmentComponent>();
	if (!EquipComp) return;

	AEquipmentBase* EquippedWeapon = EquipComp->GetCurrentItem();
	if (!IsValid(EquippedWeapon) || !EquippedWeapon->Implements<UProjectileProvider>()) return;

	TSubclassOf<AActor> ProjectileToSpawn = IProjectileProvider::Execute_GetCurrentProjectileClass(EquippedWeapon);
	if (!ProjectileToSpawn) return;
	
	FTransform SocketTransform = IProjectileProvider::Execute_GetProjectileSpawnTransform(EquippedWeapon);
	FVector SpawnLocation = Avatar->GetActorLocation();
	SpawnLocation = SocketTransform.GetLocation();

	FRotator SpawnRotation;

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

		// Angling the projectile from the socket to the target
		SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetLocation);
	}
	// --- AI LOGIC: Predictive Aiming ---
	else if (AAIController* AIController = Cast<AAIController>(AvatarPawn->GetController()))
	{
		const AActor* TargetActor = AIController->GetFocusActor();
      
		if (IsValid(TargetActor))
		{
			// Extracting the projectiles speed
			float ProjectileSpeed = 5000.0f; // Fallback speed
			if (const AActor* ProjectileCDO = ProjectileToSpawn->GetDefaultObject<AActor>())
			{
				if (const UProjectileMovementComponent* MoveComp = ProjectileCDO->FindComponentByClass<UProjectileMovementComponent>())
				{
					ProjectileSpeed = MoveComp->InitialSpeed;
				}
			}

			const FVector TargetLoc = TargetActor->GetActorLocation();
			const FVector TargetVel = TargetActor->GetVelocity();

			// Two-pass iterative prediction to find the interception point
			float Distance = FVector::Dist(SpawnLocation, TargetLoc);
			float TimeToImpact = Distance / FMath::Max(ProjectileSpeed, 1.0f);
         
			// First prediction pass
			FVector PredictedLoc = TargetLoc + (TargetVel * TimeToImpact);
         
			// Second prediction pass (refines accuracy based on the new predicted distance)
			Distance = FVector::Dist(SpawnLocation, PredictedLoc);
			TimeToImpact = Distance / FMath::Max(ProjectileSpeed, 1.0f);
			PredictedLoc = TargetLoc + (TargetVel * TimeToImpact);

			// Aim at the predicted future location
			SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, PredictedLoc);
		}
	}
	
	// --- MUZZLE OBSTRUCTION CHECK ---
	// Prevent shooting through walls by checking if the socket is clipped inside geometry
	FVector SocketLocation = SocketTransform.GetLocation();
	FVector PlayerCenter = Avatar->GetActorLocation();
	FHitResult ObstructionHit;
	FCollisionQueryParams ObstructionParams;
	ObstructionParams.AddIgnoredActor(Avatar);

	// Trace from the center of the player (safe) to the bow socket (potentially inside a wall)
	if (GetWorld()->LineTraceSingleByChannel(
		ObstructionHit, 
		PlayerCenter,
		SocketLocation, 
		ECC_Visibility, 
		ObstructionParams))
	{
		// The bow is inside a wall. 
		// Move the spawn point to the surface of the wall, pulled back a bit
		SpawnLocation = ObstructionHit.ImpactPoint + (ObstructionHit.ImpactNormal * 5.f);
		
		// Changing spawn rotation to be where the player feels like theyre aiming.
		// Avoids the arrow being rotated weirdly when stuck to the wall we are shooting at
		if (AController* Controller = AvatarPawn->GetController())
		{
			SpawnRotation = Controller->GetControlRotation();
		}
	}
	// --- MUZZLE OBSTRUCTION CHECK ENDS ---

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = AvatarPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(ProjectileToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

	// We dont call EndAbility here because the wrapper handles it via 
	// the Montage Completed delegate, but we could in order to stop it early.
}

void UGA_ChargedProjectile::OnAIReleaseEventReceived(FGameplayEventData Payload)
{
	// Calculate how long the AI held the charge
	const float TimeHeld = GetWorld()->GetTimeSeconds() - AIChargeStartTime;
    
	// Calling same logic as player
	OnInputReleased(TimeHeld);
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