// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrowProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AArrowProjectile::AArrowProjectile()
{
	// Arrows usually fly a bit faster and flatter than default projectiles
	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = 5000.f;
		ProjectileMovement->MaxSpeed = 5000.f;
		ProjectileMovement->ProjectileGravityScale = 0.5f; // Less drop
	}
}

void AArrowProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Let the base class handle damage (GAS) and visual effects
	Super::OnProjectileHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	// Stop the physics simulation immediately
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->SetComponentTickEnabled(false);
	}

	// No collision on stuck arrows
	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Sink the arrow slightly into the surface
	FVector ForwardVector = GetActorForwardVector();
	AddActorWorldOffset(ForwardVector * PenetrationDepth);

	// Attach the arrow to the object it hit (Crucial for moving targets like guards)
	if (OtherComp)
	{
		FAttachmentTransformRules AttachRules(EAttachmentRule::KeepWorld, true);
		AttachToComponent(OtherComp, AttachRules, Hit.BoneName);
	}

	// Extend the lifespan on hit so the arrow stays in the wall/guard for a minute before despawning
	SetLifeSpan(60.0f);
}