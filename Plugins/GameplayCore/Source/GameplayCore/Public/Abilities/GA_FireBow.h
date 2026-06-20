// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreGameplayAbility.h"
#include "GA_FireBow.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UGA_FireBow : public UCoreGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_FireBow();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// The implementation for when your AnimNotify triggers
	virtual void OnMontageEventReceived_Implementation(FGameplayEventData Payload) override;

	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AActor> ArrowClass; // You can change this to your specific Arrow class

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DrawMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FGameplayTag FireEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DrawingEffectClass;

private:
	// The handle to the specific instance of the effect applied
	FActiveGameplayEffectHandle ActiveEffectHandle;
};