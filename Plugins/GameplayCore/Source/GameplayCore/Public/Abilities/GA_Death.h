// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreGameplayAbility.h"
#include "GA_Death.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UGA_Death : public UCoreGameplayAbility
{
	GENERATED_BODY()

	UGA_Death();

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};
