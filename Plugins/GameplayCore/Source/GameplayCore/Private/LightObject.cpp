// Fill out your copyright notice in the Description page of Project Settings.


#include "LightObject.h"
#include "AbilitySystemComponent.h"
#include "Components/PointLightComponent.h"
#include "NiagaraComponent.h"

ALightObject::ALightObject()
{
	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLightComponent->SetupAttachment(RootComponent);

	ParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireParticles"));
	ParticleComponent->SetupAttachment(RootComponent);
}

void ALightObject::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Bind to the ASC tag event. EGameplayTagEventType::NewOrRemoved ensures 
	// this only fires when the tag goes from 0->1 or 1->0, saving performance.
	if (AbilitySystemComponent && LitStateTag.IsValid())
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(LitStateTag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &ALightObject::OnLitTagChanged);
	}
}

void ALightObject::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the runtime state is initially dark before GAS takes over
	PointLightComponent->SetVisibility(false);
	ParticleComponent->Deactivate();

	if (HasAuthority() && bStartsLit && LitGameplayEffect)
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
		ParticleComponent->Deactivate();
	}
}