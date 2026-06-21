// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreGameplayAbility.h"
#include "GA_Zoom.generated.h"

class UCameraModifier_Zoom;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UGA_Zoom : public UCoreGameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	/** Zoom amount (in FOV) to be passed to CameraModifier_Zoom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float TargetFOV = 45.0f;

	/** Zoom speed to be passed to CameraModifier_Zoom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	float ZoomSpeed = 12.0f;

protected:
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	/** Zoom GE */
	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
	TSubclassOf<UGameplayEffect> ZoomingEffectClass;

	/** Camera modifier for zooming */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraModifier_Zoom> ZoomModifierClass;

	UPROPERTY()
	class UCameraModifier_Zoom* ActiveZoomModifier;

private:
	// The handle to the specific instance of the effect applied
	FActiveGameplayEffectHandle ActiveEffectHandle;
};
