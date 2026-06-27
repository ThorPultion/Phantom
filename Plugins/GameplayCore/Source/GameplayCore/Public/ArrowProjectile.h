// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreProjectile.h"
#include "ArrowProjectile.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API AArrowProjectile : public ACoreProjectile
{
	GENERATED_BODY()

public:
	AArrowProjectile();
	
protected:
	/** Overriding the base class hit logic */
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	/** How deep should the arrow sink into the target visually? */
	UPROPERTY(EditDefaultsOnly, Category = "Arrow")
	float PenetrationDepth = 10.0f;

};