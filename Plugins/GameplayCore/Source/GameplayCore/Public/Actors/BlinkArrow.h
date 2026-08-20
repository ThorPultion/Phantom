// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ArrowProjectile.h"
#include "BlinkArrow.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API ABlinkArrow : public AArrowProjectile
{
	GENERATED_BODY()
	
protected:
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
