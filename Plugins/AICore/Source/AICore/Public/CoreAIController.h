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
	
	/** Location the target was last seen */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	FVector LastKnownLocation = FVector::ZeroVector;

	/** Used to check for staleness */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	float TimeLastSeen = 0.0f;

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
	
	/** The AIs current target (Looking, chasing, investigating, etc) */
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Memory")
	TObjectPtr<AActor> CurrentTargetActor;

	/** The state the current target is proposing that the AI should be in */
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Memory")
	FGameplayTag CurrentTargetTag;
	
	/** The AIs current movement goal, usually based on TargetActor */
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadWrite, Category = "Memory", meta = (AllowPrivateAccess = "true"))
	FVector CurrentMovementGoal = FVector::ZeroVector;
	
	/** The location the AI is currently focusing on */
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Memory")
	FVector TargetLastKnownLocation = FVector::ZeroVector;
	
	void SetTarget(AActor* NewTarget);

	virtual FGenericTeamId GetGenericTeamId() const override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logic")
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Perception")
	TObjectPtr<UCoreAIPerceptionComponent> CorePerceptionComponent;

	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

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
};
