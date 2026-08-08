// Fill out your copyright notice in the Description page of Project Settings.


#include "CorePlayerState.h"
#include "CoreAbilitySystemComponent.h"
#include "CoreAttributeSet.h"
#include "Net/UnrealNetwork.h"

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

void ACorePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACorePlayerState, GatheredGold);
}

void ACorePlayerState::AddGold(const float Amount)
{
	if (HasAuthority())
	{
		GatheredGold += Amount;
		
		OnGoldChangedDelegate.Broadcast(GatheredGold);
	}
}

void ACorePlayerState::OnRep_GatheredGold(float OldGold) const
{
	OnGoldChangedDelegate.Broadcast(GatheredGold);
}
