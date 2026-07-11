// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UAIAttributeSet::UAIAttributeSet()
{
	// Fallback attribute initialization
	InitDetectionLevel(0.0f);
	InitMaxDetection(100.0f);
}

void UAIAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Ensures the attributes replicate to clients for UI updates
	DOREPLIFETIME_CONDITION_NOTIFY(UAIAttributeSet, DetectionLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAIAttributeSet, MaxDetection, COND_None, REPNOTIFY_Always);
}

void UAIAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp the Detection Level between 0 and MaxDetection
	if (Attribute == GetDetectionLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxDetection());
	}
}

void UAIAttributeSet::OnRep_DetectionLevel(const FGameplayAttributeData& OldDetectionLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAIAttributeSet, DetectionLevel, OldDetectionLevel);
}

void UAIAttributeSet::OnRep_MaxDetection(const FGameplayAttributeData& OldMaxDetection)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAIAttributeSet, MaxDetection, OldMaxDetection);
}