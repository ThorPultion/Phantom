// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreGameplayAbility.h"
#include "GA_ChargedProjectile.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UGA_ChargedProjectile : public UCoreGameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// The implementation for when AnimNotify triggers
	virtual void OnMontageEventReceived_Implementation(FGameplayEventData Payload) override;

	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	/** Montage for charging the projectile */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ChargeMontage;

	/** Montage for when input is released after MinChargeTime amount of time */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ReleaseMontage;

	/** Tag that needs to match with AnimNotify tag to spawn the projectile */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FGameplayTag SpawnEventTag;

	/** Minimum time required for projectile to be spawned */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float MinChargeTime = 0.2f;

	/** GE for while user is charging the projectile */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> ChargingEffectClass;

private:
	// The handle to the specific instance of the effect applied
	FActiveGameplayEffectHandle ActiveEffectHandle;
};