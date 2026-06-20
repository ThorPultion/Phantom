// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CoreAbilitySystemComponent.generated.h"

class CoreAbilitySet;
class CoreGameplayAbility;
/**
 * 
 */
UCLASS()
class GASCORE_API UCoreAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	/** Grants ability asynchronously */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abilities")
	void GrantAbilityAsync(const FCoreGameplayAbilityBindInfo& BindInfo);

	/** Grants ability set asynchronously */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abilities")
	void GrantAbilitySetAsync(UCoreAbilitySet* AbilitySet);

private:
	/** Helper function for async load completion delegate binding */
	void OnBindInfoAsyncLoadComplete(FCoreGameplayAbilityBindInfo BindInfo);

	/** Helper function for async load completion delegate binding */
	void OnAbilitySetAsyncLoadComplete(UCoreAbilitySet* LoadedAbilitySet);

	/** Actually grants the abilities when class and input have been fetched from struct */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abilities")
	void GrantAbilityWithInput(TSubclassOf<UCoreGameplayAbility> AbilityClass, FGameplayTag InputTag);

public:

	/** Remove ability */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abilities")
	void RemoveAbilityByClass(TSubclassOf<UCoreGameplayAbility> AbilityClass);

	/** Removes all abilities contained within an ability set */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abilities")
	void RemoveAbilitySet(UCoreAbilitySet* AbilitySet);

	/** Remove all abilities */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Abilities")
	void RemoveAllAbilities();
};
