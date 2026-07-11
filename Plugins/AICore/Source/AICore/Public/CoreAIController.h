// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "CoreAIController.generated.h"

class UStateTreeAIComponent;
class UCoreAIPerceptionComponent;
class ACoreAICharacter;
class UAbilitySystemComponent;
struct FAIStimulus;
struct FOnAttributeChangeData;

USTRUCT(BlueprintType)
struct FPerceivedData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	FGameplayTag DesiredStateTag;

	// This operator overload allows easy use of certain find functions by just passing in an AActor pointer
	bool operator==(const AActor* OtherActor) const
	{
		return TargetActor == OtherActor;
	}
};

/**
 * 
 */
UCLASS()
class AICORE_API ACoreAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	ACoreAIController();

	// This AIs attitude towards other
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logic")
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception")
	TObjectPtr<UCoreAIPerceptionComponent> CorePerceptionComponent;

	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/** The AIs current movement goal */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Memory", meta = (AllowPrivateAccess = "true"))
	FVector CurrentTargetLocation = FVector::ZeroVector;

	/** Known target actors and their supplied tags */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Memory", meta = (AllowPrivateAccess = "true"))
	TArray<FPerceivedData> KnownTargets;

	/** Deciding the best target from all known targets */
	void EvaluateBestTarget();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// Cached references to avoid casting during gameplay
	UPROPERTY()
	TObjectPtr<ACoreAICharacter> ControlledCharacter;

	// The ASC belonging to the controlled character
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	void OnDetectionLevelChanged(const FOnAttributeChangeData& Data);
	FDelegateHandle DetectionDelegateHandle;

	void UpdateTargetState(AActor* Target, FGameplayTag NewStateTag);

public:

	/** The AIs current target (Looking, chasing, investigating, etc) */
	UPROPERTY(BlueprintReadOnly, Category = "Memory")
	TObjectPtr<AActor> CurrentTargetActor;

	/** The state the current target is proposing that the AI should be in */
	UPROPERTY(BlueprintReadOnly, Category = "Memory")
	FGameplayTag CurrentTargetTag;
};
