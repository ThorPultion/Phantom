// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GASCoreTags.h"
#include "Perception/AISense_Damage.h"

UCoreAttributeSet::UCoreAttributeSet()
{
	// Initialization fallbacks. 
	// These are overridden using a Gameplay Effect and a DataTable.
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitEnergy(100.f);
	InitMaxEnergy(100.f);
	InitMovementSpeed(600.f);
}

void UCoreAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoreAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
}

void UCoreAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamping the proposed new value between 0 and the current Max
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxEnergy());
	}
	else if (Attribute == GetMovementSpeedAttribute())
	{
		NewValue = FMath::Max<float>(NewValue, 0.0f);
	}
}

void UCoreAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		TryReportDamageSense(Data);
		
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

		// Gameplay events for ability triggering
		if (GetHealth() <= 0.0f)
		{
			HandleDeath(Data);
		}
		else
		{
			TryTriggerKnockback(Data);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetEnergyAttribute())
	{
		SetEnergy(FMath::Clamp(GetEnergy(), 0.f, GetMaxEnergy()));
	}
}

void UCoreAttributeSet::HandleDeath(const FGameplayEffectModCallbackData& Data) const
{
	float ImpactForce = 0.f;

	// Checking if the killing blow was also a knockback effect
	const FGameplayTagContainer& AssetTags = Data.EffectSpec.Def->GetAssetTags();
	if (AssetTags.HasTagExact(GASCoreTags::Effect_Knockback))
	{
		// Extracting the knockback effects force
		ImpactForce = Data.EffectSpec.GetSetByCallerMagnitude(GASCoreTags::Data_Magnitude_Force, false, 0.f);
	}

	// Sending gameplay event to trigger death ability
	FGameplayEventData Payload = CreateEventPayload(Data, ImpactForce);
	Payload.EventTag = GASCoreTags::Event_Death;
	GetOwningAbilitySystemComponent()->HandleGameplayEvent(Payload.EventTag, &Payload);
}

void UCoreAttributeSet::TryTriggerKnockback(const FGameplayEffectModCallbackData& Data) const
{
	// Checking if the GE is supposed to knock back
	const FGameplayTagContainer& AssetTags = Data.EffectSpec.Def->GetAssetTags();
	if (AssetTags.HasTagExact(GASCoreTags::Effect_Knockback))
	{
		float ImpactForce = Data.EffectSpec.GetSetByCallerMagnitude(GASCoreTags::Data_Magnitude_Force, false, 0.f);
		if (ImpactForce > 0.f)
		{
			// Sending gameplay event to trigger knockback ability
			FGameplayEventData Payload = CreateEventPayload(Data, ImpactForce);
			Payload.EventTag = GASCoreTags::Event_Movement_Knockback;
			GetOwningAbilitySystemComponent()->HandleGameplayEvent(Payload.EventTag, &Payload);
		}
	}
}

void UCoreAttributeSet::TryReportDamageSense(const FGameplayEffectModCallbackData& Data) const
{
	// We only report if the change was negative (Damage, not Healing)
	const float Magnitude = Data.EvaluatedData.Magnitude;
	if (Magnitude >= 0.f) return;
	
	const float DamageTaken = FMath::Abs(Magnitude);
	AActor* TargetActor = GetOwningActor();
	AActor* InstigatorActor = Data.EffectSpec.GetEffectContext().GetInstigator();

	// Ensuring both actors exist and ensure this isnt a self damage event
	if (IsValid(TargetActor) && IsValid(InstigatorActor) && TargetActor != InstigatorActor)
	{
		// Attempting to grab the exact hit location on the body
		FVector HitLocation = TargetActor->GetActorLocation();
		if (const FHitResult* HitResult = Data.EffectSpec.GetEffectContext().GetHitResult())
		{
			HitLocation = HitResult->ImpactPoint;
		}

		// Broadcasting to the AI Perception System
		UAISense_Damage::ReportDamageEvent(
		   TargetActor->GetWorld(),
		   TargetActor,
		   InstigatorActor,
		   DamageTaken,
		   InstigatorActor->GetActorLocation(),
		   HitLocation
		);
	}
}

FGameplayEventData UCoreAttributeSet::CreateEventPayload(const FGameplayEffectModCallbackData& Data, const float EventMagnitude) const
{
	FGameplayEventData Payload;
	Payload.Instigator = Data.EffectSpec.GetEffectContext().GetOriginalInstigator();
	Payload.Target = GetOwningActor();
	Payload.EventMagnitude = EventMagnitude;

	if (const FHitResult* HitResult = Data.EffectSpec.GetContext().GetHitResult())
	{
		Payload.ContextHandle = Data.EffectSpec.GetContext();
		Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(*HitResult);
	}

	return Payload;
}

// --- RepNotifies ---

void UCoreAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Health, OldHealth);
}

void UCoreAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, MaxHealth, OldMaxHealth);
}

void UCoreAttributeSet::OnRep_Energy(const FGameplayAttributeData& OldEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, Energy, OldEnergy);
}

void UCoreAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, MaxEnergy, OldMaxEnergy);
}

void UCoreAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoreAttributeSet, MovementSpeed, OldMovementSpeed);
}
