// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "CoreProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UGameplayEffect;

UCLASS()
class GAMEPLAYCORE_API ACoreProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoreProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** The main collision shape */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	/** The visual representation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	/** Handles movement, gravity, and bouncing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	/** * Universal hit function.
	 * Must be a UFUNCTION to bind to the collision components delegate!
	 */
	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** * Blueprint hook for visual effects on impact.
	 * Allows designers to spawn explosion particles or sounds without touching C++.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
	void OnImpact(const FHitResult& Hit);

	/** The Gameplay Effect applied to the target on hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> ImpactDamageEffect;

	/** The Gameplay Cue tag to trigger sounds and particles on impact */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	FGameplayTag ImpactGameplayCue;
};