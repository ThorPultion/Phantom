// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreCharacterBase.h"
#include "CoreAICharacter.generated.h"

class UAIStatePriorityData;
class UAIAttributeSet;
class UCoreWidgetComponent;

UCLASS()
class AICORE_API ACoreAICharacter : public ACoreCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACoreAICharacter();

	// Server side possession initialization for AI
	virtual void PossessedBy(AController* NewController) override;

	/** Defines which states take priority */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<UAIStatePriorityData> PriorityData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitAbilitySystem();

	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAIAttributeSet> AIAttributeSet;

	virtual void SetupAttributes() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UCoreWidgetComponent> DetectionWidgetComponent;

protected:
	FDelegateHandle CombatTagDelegateHandle;

	void OnCombatTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
