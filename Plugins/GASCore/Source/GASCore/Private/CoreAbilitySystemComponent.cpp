// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAbilitySystemComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "CoreGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "CoreAbilitySet.h"

void UCoreAbilitySystemComponent::GrantAbilityAsync(const FCoreGameplayAbilityBindInfo& BindInfo)
{
	if (GetOwnerRole() != ROLE_Authority || BindInfo.AbilityClass.IsNull()) return;

	// Load the CoreGameplayAbility from the struct
	FSoftObjectPath AssetPath = BindInfo.AbilityClass.ToSoftObjectPath();

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		AssetPath,
		FStreamableDelegate::CreateUObject(this, &UCoreAbilitySystemComponent::OnBindInfoAsyncLoadComplete, BindInfo)
	);
}

void UCoreAbilitySystemComponent::OnBindInfoAsyncLoadComplete(FCoreGameplayAbilityBindInfo BindInfo)
{
	// Resolving the class safely
	UClass* LoadedAbilityClass = BindInfo.AbilityClass.Get();

	if (LoadedAbilityClass)
	{
		// Pass to synchronous GrantAbility
		GrantAbilityWithInput(LoadedAbilityClass, BindInfo.InputTag);
	}
}

void UCoreAbilitySystemComponent::GrantAbilitySetAsync(UCoreAbilitySet* AbilitySet)
{
	if (GetOwnerRole() != ROLE_Authority || !AbilitySet) return;

	TArray<FSoftObjectPath> AssetPaths;

	// Gather all the valid paths from the Data Asset
	for (const FCoreGameplayAbilityBindInfo& BindInfo : AbilitySet->Abilities)
	{
		if (!BindInfo.AbilityClass.IsNull())
		{
			// AddUnique prevents loading the same ability twice if you accidentally duplicated it in the data asset
			AssetPaths.AddUnique(BindInfo.AbilityClass.ToSoftObjectPath());
		}
	}

	// If theres nothing to load, bail out early
	if (AssetPaths.IsEmpty()) return;

	// Request the batch load
	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		AssetPaths,
		FStreamableDelegate::CreateUObject(this, &UCoreAbilitySystemComponent::OnAbilitySetAsyncLoadComplete, AbilitySet)
	);
}

void UCoreAbilitySystemComponent::OnAbilitySetAsyncLoadComplete(UCoreAbilitySet* LoadedAbilitySet)
{
	// Safety check just in case the pointer went stale
	if (!LoadedAbilitySet) return;

	// Because we are in this callback, we are guaranteed that EVERY ability in the set is now in memory
	for (const FCoreGameplayAbilityBindInfo& BindInfo : LoadedAbilitySet->Abilities)
	{
		// Resolve the class
		UClass* LoadedAbilityClass = BindInfo.AbilityClass.Get();

		if (LoadedAbilityClass)
		{
			// Pass to synchronous GrantAbility
			GrantAbilityWithInput(LoadedAbilityClass, BindInfo.InputTag);
		}
	}
}

void UCoreAbilitySystemComponent::GrantAbilityWithInput(TSubclassOf<UCoreGameplayAbility> AbilityClass, FGameplayTag InputTag)
{
	if (!AbilityClass) return;

	// Explicitly casting the UCoreGameplayAbility to UGameplayAbility because... (line 88)
	TSubclassOf<UGameplayAbility> BaseAbilityClass = *AbilityClass;

	// ... FGameplayAbilitySpec constructor only accepts TSubclassOf<UGameplayAbility>
	FGameplayAbilitySpec Spec(BaseAbilityClass, 1, INDEX_NONE, GetAvatarActor());

	if (InputTag.IsValid())
	{
		Spec.GetDynamicSpecSourceTags().AddTag(InputTag);
	}

	// Call the native GAS function
	GiveAbility(Spec);
}

void UCoreAbilitySystemComponent::RemoveAbilityByClass(TSubclassOf<UCoreGameplayAbility> AbilityClass)
{
	if (GetOwnerRole() != ROLE_Authority || !AbilityClass) return;

	TArray<FGameplayAbilitySpec>& CurrentActivatableAbilities = GetActivatableAbilities();

	// Check current abilities for a match to the ability we want to remove
	for (int32 i = CurrentActivatableAbilities.Num() - 1; i >= 0; i--)
	{
		if (CurrentActivatableAbilities[i].Ability && CurrentActivatableAbilities[i].Ability->GetClass() == AbilityClass)
		{
			ClearAbility(CurrentActivatableAbilities[i].Handle);
		}
	}
}

void UCoreAbilitySystemComponent::RemoveAbilitySet(UCoreAbilitySet* AbilitySet)
{
	if (!AbilitySet) return;

	for (const FCoreGameplayAbilityBindInfo& BindInfo : AbilitySet->Abilities)
	{
		// Resolve the Soft Pointer to a Hard Pointer using .Get()
		RemoveAbilityByClass(BindInfo.AbilityClass.Get());
	}
}

void UCoreAbilitySystemComponent::RemoveAllAbilities()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ClearAllAbilities();
	}
}