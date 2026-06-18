// Fill out your copyright notice in the Description page of Project Settings.


#include "CorePlayerState.h"
#include "CoreAbilitySystemComponent.h"
#include "CoreAttributeSet.h"

ACorePlayerState::ACorePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UCoreAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed replication mode is ideal for player controlled actors in multiplayer
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UCoreAttributeSet>(TEXT("AttributeSet"));

	// PlayerState NetUpdateFrequency is very low by default
	SetNetUpdateFrequency(100.0f);
	// Allows engine to dynamically lower frequency when replicated properties arent changing
	SetMinNetUpdateFrequency(2.0f);
}

UAbilitySystemComponent* ACorePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
