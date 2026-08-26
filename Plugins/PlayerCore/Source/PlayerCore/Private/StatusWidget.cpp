// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CoreAttributeSet.h"

void UStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const APawn* OwningPawn = GetOwningPlayerPawn();
	
	if (!IsValid(OwningPawn)) return;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningPawn);
	
	if (!IsValid(ASC)) return;
	
	ASC->GetGameplayAttributeValueChangeDelegate(UCoreAttributeSet::GetHealthAttribute()).AddUObject(
		this, &UStatusWidget::HandleHealthChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(UCoreAttributeSet::GetMaxHealthAttribute()).AddUObject(
		this, &UStatusWidget::HandleMaxHealthChanged);
			
	CurrentHealth = ASC->GetNumericAttribute(UCoreAttributeSet::GetHealthAttribute());
	MaxHealth = ASC->GetNumericAttribute(UCoreAttributeSet::GetMaxHealthAttribute());
			
	RecalculateHealthPercent();
}

void UStatusWidget::HandleMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	// Updating max health to get accurate health percent for UI
	MaxHealth = Data.NewValue;
	
	RecalculateHealthPercent();
}

void UStatusWidget::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentHealth = Data.NewValue;
	
	RecalculateHealthPercent();
}

void UStatusWidget::RecalculateHealthPercent()
{
	// Avoiding divide by zero
	const float HealthPercent = (MaxHealth > 0.f) ? (CurrentHealth / MaxHealth) : 0.f;

	// UI update
	OnHealthUpdated(HealthPercent);
}
