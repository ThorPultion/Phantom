// Fill out your copyright notice in the Description page of Project Settings.


#include "Cues/GC_DeathRagdoll.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GASCoreTags.h"
#include "CoreCharacterBase.h"

bool UGC_DeathRagdoll::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	ACharacter* AvatarCharacter = Cast<ACharacter>(MyTarget);
	if (!AvatarCharacter)
	{
		return false;
	}

	if (UCharacterMovementComponent* MoveComp = AvatarCharacter->GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_None);
		AvatarCharacter->SetReplicateMovement(false);
	}

	// Disable capsule collision and movement component
	if (UCapsuleComponent* Capsule = AvatarCharacter->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	USkeletalMeshComponent* Mesh = AvatarCharacter->GetMesh();
	if (!Mesh)
	{
		return false;
	}

	Mesh->SetSimulatePhysics(true);
	Mesh->SetOwnerNoSee(false);
	if (ACoreCharacterBase* BaseCharacter = Cast<ACoreCharacterBase>(AvatarCharacter))
	{
		if (USkeletalMeshComponent* FPMesh = BaseCharacter->GetFirstPersonMesh())
		{
			FPMesh->SetVisibility(false);
		}
	}

	// Apply the force thats been passed here through payload
	if (Parameters.RawMagnitude > 0.f)
	{
		FVector ImpulseDirection = FVector::ZeroVector;
		FVector ImpactLocation = AvatarCharacter->GetActorLocation();
		FName HitBoneName = NAME_None;

		// Taking hit result from payload
		if (const FHitResult* HitResult = Parameters.EffectContext.GetHitResult())
		{
			// Get the exact bone that was hit (e.g., "head", "upperarm_l")
			if (HitResult->BoneName != NAME_None)
			{
				HitBoneName = HitResult->BoneName;
			}

			ImpactLocation = HitResult->ImpactPoint;

			// Calculate direction based on the line trace/projectile path
			ImpulseDirection = (HitResult->TraceEnd - HitResult->TraceStart).GetSafeNormal();

			// If trace data wasnt provided (e.g., AoE overlaps), fallback to impact normal
			if (ImpulseDirection.IsNearlyZero())
			{
				ImpulseDirection = -HitResult->ImpactNormal;
			}
		}
		// If no hit data exists
		else if (const AActor* Instigator = Parameters.Instigator.Get())
		{
			ImpulseDirection = (AvatarCharacter->GetActorLocation() - Instigator->GetActorLocation()).GetSafeNormal();
		}

		// Adding upwards bias for a little lift
		ImpulseDirection.Z = FMath::Max(ImpulseDirection.Z, 0.25f);
		ImpulseDirection.Normalize();

		FVector FinalImpulse = ImpulseDirection * Parameters.RawMagnitude;

		// Use AddImpulseAtLocation to apply the force exactly where the bullet/weapon struck, 
		// creating accurate rotational torque on the ragdoll.
		Mesh->AddImpulseAtLocation(FinalImpulse, ImpactLocation, HitBoneName);
	}

	return true;
}