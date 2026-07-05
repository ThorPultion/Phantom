// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGASObject.h"
#include "AbilitySystemComponent.h"
#include "CoreAttributeSet.h"
#include "CoreAbilitySystemComponent.h"
#include "GASInitData.h"
#include "GASCoreTags.h"

ACoreGASObject::ACoreGASObject()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MeshComponent->SetSimulatePhysics(true);
	SetReplicatingMovement(true);

	AbilitySystemComponent = CreateDefaultSubobject<UCoreAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Minimal is the best practice for AI and Objects. 
	// It only replicates GameplayEffects to clients if it absolutely has to
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UCoreAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ACoreGASObject::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACoreGASObject::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// For standard non-player actors, the OwnerActor and AvatarActor are the exact same object
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// Granting object starting abilities
		UCoreAbilitySystemComponent* CoreASC = Cast<UCoreAbilitySystemComponent>(AbilitySystemComponent);
		if (HasAuthority() && CoreASC && GASInitData)
		{
			CoreASC->GrantAbilitySetAsync(GASInitData->StartingAbilities);
		}

		// Initializing attributes
		if (GASInitData->InitializationEffect)
		{
			FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
			ContextHandle.AddInstigator(this, this);

			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GASInitData->InitializationEffect, 1.f, ContextHandle);

			if (SpecHandle.IsValid())
			{
				// Pulling stats from data asset
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(GASCoreTags::Data_Attribute_MaxHealth, GASInitData->MaxHealth);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(GASCoreTags::Data_Attribute_MaxEnergy, GASInitData->MaxEnergy);

				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void ACoreGASObject::BeginPlay()
{
	Super::BeginPlay();

	// You can grant baseline passive abilities here if the object needs them
}

