// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreGameplayAbility.h"
#include "GameplayEffect.h"
#include "GA_Zoom.generated.h"

class UCameraModifier_Zoom;

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UGA_Zoom : public UCoreGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Zoom();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
	TSubclassOf<UGameplayEffect> ZoomingEffectClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraModifier_Zoom> ZoomModifierClass;

	UPROPERTY()
	class UCameraModifier_Zoom* ActiveZoomModifier;

private:
	// The handle to the specific instance of the effect applied
	FActiveGameplayEffectHandle ActiveEffectHandle;
};
