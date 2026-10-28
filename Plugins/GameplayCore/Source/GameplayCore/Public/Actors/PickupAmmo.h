// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupBase.h"
#include "PickupAmmo.generated.h"

class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API APickupAmmo : public APickupBase
{
	GENERATED_BODY()
	
public:
	APickupAmmo();
	
	virtual void Interact_Implementation(AActor* Interactor) override;
	
	/** Ammos defining particle effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraComponent> ParticleSystem;
	
	virtual void OnRep_ItemData() override;
};
