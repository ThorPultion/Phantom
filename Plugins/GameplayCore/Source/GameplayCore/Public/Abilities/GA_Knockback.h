// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreGameplayAbility.h"
#include "GA_Knockback.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UGA_Knockback : public UCoreGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Knockback();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	// The callback that fires when the root motion timer expires
	UFUNCTION()
	void OnKnockbackFinished();
	
	/** Multiplier required to impulses that use mass */
	UPROPERTY(EditDefaultsOnly, Category = "Force")
	float NonCharacterImpulseMultiplier = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Force")
	float ImpulseLiftModifier = 0.25f;
};