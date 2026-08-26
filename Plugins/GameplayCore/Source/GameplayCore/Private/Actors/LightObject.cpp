// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LightObject.h"
#include "AbilitySystemComponent.h"
#include "Components/PointLightComponent.h"
#include "NiagaraComponent.h"
#include "GASCoreTags.h"

ALightObject::ALightObject()
{
	PrimaryActorTick.bCanEverTick = false;

	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLightComponent->SetupAttachment(RootComponent);
	
	PointLightComponent->SetMobility(EComponentMobility::Stationary);

	ParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ParticleSystem"));
	ParticleComponent->SetupAttachment(RootComponent);
}

void ALightObject::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Bind to the ASC tag event. EGameplayTagEventType::NewOrRemoved ensures 
	// this only fires when the tag goes from 0->1 or 1->0, saving performance.
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(GASCoreTags::State_LightSource_Lit, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &ALightObject::OnLitTagChanged);
	}
}

void ALightObject::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the runtime state is initially dark before GAS takes over
	PointLightComponent->SetVisibility(false);
	ParticleComponent->Deactivate();

	if (HasAuthority() && bStartsLit && LitGameplayEffect && IsValid(AbilitySystemComponent))
	{
		// Applying this GE grants the LitStateTag, which instantly triggers OnLitTagChanged
		FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		ContextHandle.AddInstigator(this, this);

		UGameplayEffect* BaseEffect = LitGameplayEffect->GetDefaultObject<UGameplayEffect>();
		AbilitySystemComponent->ApplyGameplayEffectToSelf(BaseEffect, 1.f, ContextHandle);
	}
}

void ALightObject::OnLitTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// NewCount > 0 means the tag is present (the torch is lit)
	// NewCount == 0 means the tag is gone (extinguished)
	bool bIsLit = (NewCount > 0);

	PointLightComponent->SetVisibility(bIsLit);

	if (bIsLit)
	{
		ParticleComponent->Activate();
	}
	else
	{
		ParticleComponent->DeactivateImmediate();
	}
}