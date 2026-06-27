// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CoreGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASCORE_API UCoreGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UCoreGameplayAbility();

	/** UI icon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UTexture2D> AbilityIcon;
	
protected:
	/** Play montage, nothing else */
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAbilityMontage(UAnimMontage* MontageToPlay);

	/** Play montage and listen for notify */
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAbilityMontageAndWaitForEvent(UAnimMontage* MontageToPlay, FGameplayTag EventTag);

	/** What happens when notify is received */
	UFUNCTION(BlueprintNativeEvent, Category = "Animation")
	void OnMontageEventReceived(FGameplayEventData Payload);

	/** Prevents the ability from ending if we intentionally interrupt our own montage to play another one */
	bool bIsChainingMontages = false;

private:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();
};
