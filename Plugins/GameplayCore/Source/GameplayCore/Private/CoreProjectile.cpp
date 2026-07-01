// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "AmmoData.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GASCoreTags.h"

ACoreProjectile::ACoreProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Projectiles must replicate in a multiplayer game
	SetReplicates(true);
	SetReplicateMovement(true);

	// Setting up collider, collider is root
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &ACoreProjectile::OnProjectileHit);
	RootComponent = CollisionComponent;

	// Visuals
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Only the sphere handles collision
	ProjectileMesh->SetGenerateOverlapEvents(false);
	ProjectileMesh->CanCharacterStepUpOn = ECB_No;

	ParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(RootComponent);

	// Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true; // Crucial for arrows to point forward
	ProjectileMovement->bShouldBounce = false;

	InitialLifeSpan = 30.0f;
}

void ACoreProjectile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!ProjectileData) return;

	// Updating meshcomponent to use our meshasset
	if (ProjectileMesh)
	{
		ProjectileMesh->SetStaticMesh(ProjectileData->AmmoMesh);
	}

	if (ParticleSystem)
	{
		ParticleSystem->SetAsset(ProjectileData->AmmoParticleSystem);
	}
}

void ACoreProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* MyOwner = GetOwner())
	{
		// Ignoring owner to prevent self hit
		CollisionComponent->IgnoreActorWhenMoving(MyOwner, true);

		// Setting a timer for when the arrow can hit the owner again
		FTimerHandle ArmingTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			ArmingTimerHandle,
			this,
			&ACoreProjectile::UnIgnoreOwner,
			0.15f, // May need to change this in case of a very slow projectile
			false
		);
	}
}

void ACoreProjectile::UnIgnoreOwner()
{
	if (AActor* MyOwner = GetOwner())
	{
		// Turning collision with the owner back on.
		// If it bounces back or falls on their head now, it is a valid hit.
		CollisionComponent->IgnoreActorWhenMoving(MyOwner, false);
	}
}

void ACoreProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Let Blueprints handle the sparks/sounds locally
	OnImpact(Hit);

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ParticleSystem->Deactivate();

	// Only the server should deal damage or apply Gameplay Effects
	if (!HasAuthority() || !OtherActor) return;

	IAbilitySystemInterface* TargetInterface = Cast<IAbilitySystemInterface>(OtherActor);
	if (!TargetInterface) return;

	UAbilitySystemComponent* TargetASC = TargetInterface->GetAbilitySystemComponent();
	APawn* FiringPawn = GetInstigator();
	if (!TargetASC || !FiringPawn) return;

	IAbilitySystemInterface* FiringInterface = Cast<IAbilitySystemInterface>(FiringPawn);
	if (!FiringInterface) return;

	UAbilitySystemComponent* SourceASC = FiringInterface->GetAbilitySystemComponent();
	if (!SourceASC) return;


	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddHitResult(Hit);
	EffectContext.AddInstigator(FiringPawn, this);

	if (ProjectileData->ImpactDamageEffect && ProjectileData)
	{
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(ProjectileData->ImpactDamageEffect, 1.0f, EffectContext);
		if (SpecHandle.IsValid())
		{
			for (auto& DamageConfig : ProjectileData->DamageConfig)
			{
				SpecHandle.Data->SetSetByCallerMagnitude(DamageConfig.Key, DamageConfig.Value);

			}

			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);
		}
	}

	if (ProjectileData->ImpactGameplayCue.IsValid())
	{
		FGameplayCueParameters CueParams;
		CueParams.EffectContext = EffectContext;
		CueParams.Location = Hit.ImpactPoint;
		CueParams.Normal = Hit.ImpactNormal;

		TargetASC->ExecuteGameplayCue(ProjectileData->ImpactGameplayCue, CueParams);
	}
}