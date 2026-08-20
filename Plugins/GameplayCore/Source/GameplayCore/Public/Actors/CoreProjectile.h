// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "CoreProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UGameplayEffect;
class UNiagaraComponent;
class UAmmoData;

UCLASS()
class GAMEPLAYCORE_API ACoreProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoreProjectile();
	
	// On blueprint construction
	virtual void OnConstruction(const FTransform& Transform) override;

protected:

	/** Main collision shape */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	/** Arrow mesh component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	/** Arrow mesh asset */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAmmoData> ProjectileData;

	/** Handles movement, gravity, and bouncing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	/** * Universal hit function.
	 * Must be a UFUNCTION to bind to the collision components delegate!
	 */
	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Blueprint hook for gameplay effects on impact */
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
	void OnAuthoritativeImpact(const FHitResult& Hit);
	
	/** Blueprint hook for visual effects on impact */
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
	void OnVisualImpact(const FHitResult& Hit);

	/** Projectiles defining particle effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraComponent> ParticleSystem;

	virtual void BeginPlay() override;

	/** Letting the projectile hit the owner again */
	UFUNCTION()
	void UnIgnoreOwner();
};