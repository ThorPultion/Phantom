// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PickupAmmo.h"
#include "Components/EquipmentComponent.h"
#include "Data/AmmoData.h"
#include "Data/AmmoDefinition.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

APickupAmmo::APickupAmmo()
{
	ParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(RootComponent);
}

void APickupAmmo::Interact_Implementation(AActor* Interactor)
{
	// Only server manages inventory
	if (!HasAuthority()) return;

	if (Interactor && ItemData)
	{
		if (const UAmmoDefinition* AmmoDefinition = Cast<UAmmoDefinition>(ItemData))
		{
			if (UEquipmentComponent* EquipmentComponent = Interactor->FindComponentByClass<UEquipmentComponent>())
			{
				// Adding Ammo to the ammo storage within EquipmentComponent
				EquipmentComponent->AddAmmo(AmmoDefinition->GrantedAmmo);

				Destroy();
			}
		}
	}
}

void APickupAmmo::OnRep_ItemData()
{
	Super::OnRep_ItemData();
	
	if (ItemData)
	{
		if (const UAmmoDefinition* AmmoDefinition = Cast<UAmmoDefinition>(ItemData))
		{
			// Assign the particle system based on AmmoData
			if (IsValid(ParticleSystem))
			{
				ParticleSystem->SetAsset(AmmoDefinition->GrantedAmmo->AmmoParticleSystem);
				
				return;
			}
		}
	}
	
	if (IsValid(ParticleSystem))
	{
		// Clearing the particle system if there is no valid data
		ParticleSystem->SetAsset(nullptr);
	}
}
