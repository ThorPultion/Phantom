// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/CollectibleDefinition.h"
#include "AbilitySystemComponent.h"
#include "Interfaces/ValuableCollector.h"

void UCollectibleDefinition::CollectCollectible(AActor* Interactor) const
{
	if (!Interactor) return;

	switch (BehaviorType)
	{
	case ECollectibleBehavior::Valuable:
		{
			// Check if the interacting actor can collect valuables
			if (Interactor->Implements<UValuableCollector>())
			{
				// Passing in the worth of our valuable for the interactor to store
				IValuableCollector::Execute_AddValuable(Interactor, WorthInGold);
			}
			break;
		}
	case ECollectibleBehavior::Ability:
		{
			if (UAbilitySystemComponent* ASC = Interactor->FindComponentByClass<UAbilitySystemComponent>())
			{
				if (EffectToApply)
				{
					FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
					Context.AddInstigator(Interactor, Interactor);
					ASC->ApplyGameplayEffectToSelf(EffectToApply.GetDefaultObject(), 1.0f, Context);
				}
			}
			break;
		}
	}
}
