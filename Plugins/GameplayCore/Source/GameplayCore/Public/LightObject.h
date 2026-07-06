// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreGASObject.h"
#include "GameplayTagContainer.h"
#include "LightObject.generated.h"

class UNiagaraComponent;
class UPointLightComponent;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API ALightObject : public ACoreGASObject
{
	GENERATED_BODY()

public:
	ALightObject();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	UPointLightComponent* PointLightComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
	UNiagaraComponent* ParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Light")
	bool bStartsLit = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Light")
	TSubclassOf<UGameplayEffect> LitGameplayEffect;

	// The tag granted by the GE that we want to listen for
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Light")
	FGameplayTag LitStateTag;

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	// The callback function that fires when the tag changes
	virtual void OnLitTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
