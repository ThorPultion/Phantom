// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_SwapAmmo.h"
#include "EquipmentComponent.h"
#include "AbilitySystemComponent.h"
#include "GASCoreTags.h"

void UGA_SwapAmmo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CommitAbility(Handle, ActorInfo, ActivationInfo) && TriggerEventData)
	{
		AActor* Avatar = GetAvatarActorFromActorInfo();
		if (UEquipmentComponent* EquipComp = Avatar->FindComponentByClass<UEquipmentComponent>())
		{
			if (TriggerEventData->EventTag.MatchesTag(GASCoreTags::Input_SwapAmmo_SetIndex))
			{
				int32 IndexToSet = FMath::RoundToInt(TriggerEventData->EventMagnitude);
				EquipComp->SetItemAmmoIndex(IndexToSet);
			}
			else if (TriggerEventData->EventTag.MatchesTag(GASCoreTags::Input_SwapAmmo_Cycle))
			{
				// Converting the float from the input payload into an int32 (-1 or 1)
				int32 Direction = FMath::RoundToInt(TriggerEventData->EventMagnitude);
				EquipComp->CycleActiveItemAmmo(Direction);
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
