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
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Server side possession initialization for AI
	virtual void PossessedBy(AController* NewController) override;

	/** Defines which states take priority */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<UAIStatePriorityData> PriorityData;
	
	/** AIControllers current target actor, used for ABP replication */
	UPROPERTY(VisibleAnywhere, Replicated, Transient, BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<AActor> CurrentTargetActor;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual FGameplayTag GetPerceptionTag_Implementation(ETeamAttitude::Type ObserverAttitude, const FAIStimulus& Stimulus) override;

protected:

	virtual void InitAbilitySystem() override;

	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAIAttributeSet> AIAttributeSet;

	virtual void SetupAttributes() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UCoreWidgetComponent> DetectionWidgetComponent;

protected:
	FDelegateHandle CombatTagDelegateHandle;

	virtual void OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;
	
};
