// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CoreGASObject.h"
#include "AbilitySystemComponent.h"
#include "CoreAttributeSet.h"
#include "CoreAbilitySystemComponent.h"
#include "Data/GASInitData.h"
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
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// Granting object starting abilities
		UCoreAbilitySystemComponent* CoreASC = Cast<UCoreAbilitySystemComponent>(AbilitySystemComponent);
		if (HasAuthority() && CoreASC && GASInitData)
		{
			CoreASC->GrantAbilitySetAsync(GASInitData->StartingAbilities);
		}

		if (GASInitData)
		{
			// Initializing attributes
			if (GASInitData->InitializationEffect)
			{
				FGameplayEffectContextHandle ContextHandle = CoreASC->MakeEffectContext();
				ContextHandle.AddInstigator(this, this);

				FGameplayEffectSpecHandle SpecHandle = CoreASC->MakeOutgoingSpec(GASInitData->InitializationEffect, 1.f, ContextHandle);

				if (SpecHandle.IsValid())
				{
					// Pulling stats from data asset
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(GASCoreTags::Data_Attribute_MaxHealth, GASInitData->MaxHealth);
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(GASCoreTags::Data_Attribute_MaxEnergy, GASInitData->MaxEnergy);

					CoreASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}

			FGameplayEffectContextHandle ContextHandle = CoreASC->MakeEffectContext();
			ContextHandle.AddInstigator(this, this);

			// Applying passive effects and tags
			for (TSubclassOf<UGameplayEffect> EffectClass : GASInitData->PassiveEffects)
			{
				if (EffectClass)
				{
					UGameplayEffect* BaseEffect = EffectClass->GetDefaultObject<UGameplayEffect>();
					CoreASC->ApplyGameplayEffectToSelf(BaseEffect, 1.f, ContextHandle);
				}
			}
		}
	}
}

void ACoreGASObject::BeginPlay()
{
	Super::BeginPlay();

	// You can grant baseline passive abilities here if the object needs them
}

