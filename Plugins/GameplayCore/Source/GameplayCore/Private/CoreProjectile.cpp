// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ACoreProjectile::ACoreProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Projectiles must replicate in a multiplayer game
	SetReplicates(true);
	SetReplicateMovement(true);

	// Setting up collider, collider is root
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile")); // Make sure this profile exists in your Project Settings!
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACoreProjectile::OnProjectileHit);
	RootComponent = CollisionComponent;

	// Visuals
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Only the sphere handles collision

	// Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true; // Crucial for arrows to point forward!
	ProjectileMovement->bShouldBounce = false;
}

void ACoreProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	}

	// Optional safety net: Destroy the projectile after 10 seconds if it flies off into the void
	SetLifeSpan(10.0f);
}

void ACoreProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only the server should deal damage or apply Gameplay Effects
	if (HasAuthority())
	{
		// TODO: Here is where we will apply the GAS Gameplay Effect to the target!
	}

	// Let Blueprints handle the sparks/sounds locally
	OnImpact(Hit);

	// Default behavior: destroy on impact. 
	// (We will override this in the Arrow subclass to make them stick into walls instead)
	Destroy();
}