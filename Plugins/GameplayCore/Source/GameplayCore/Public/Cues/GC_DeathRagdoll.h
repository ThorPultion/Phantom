// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GC_DeathRagdoll.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UGC_DeathRagdoll : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

protected:

	// This is the function that runs on every client when the cue is triggered
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

	/** Multiplier required to impulses that use mass */
	UPROPERTY(EditDefaultsOnly, Category = "Impulse")
	float ImpulseMultiplier = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Impulse")
	float ImpulseLiftModifier = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FCollisionProfileName RagdollProfile = FCollisionProfileName(TEXT("Ragdoll"));;

};
