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
class UAIDetectionData;
struct FAIStimulus;
struct FOnAttributeChangeData;

USTRUCT(BlueprintType)
struct FPerceivedData
{
	GENERATED_BODY()

	/** The AIs current target */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	TObjectPtr<AActor> TargetActor;

	/** The state the current target is proposing that the AI should be in */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	FGameplayTag DesiredStateTag;
	
	/** Location the target was last seen */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	FVector LastKnownLocation = FVector::ZeroVector;

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
	
	/** The full data struct of our currently active target */
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Memory")
	FPerceivedData CurrentTargetData;
	
	/** The AIs current movement goal, usually based on TargetActor */
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadWrite, Category = "Memory", meta = (AllowPrivateAccess = "true"))
	FVector CurrentMovementGoal = FVector::ZeroVector;

	void SetTarget(AActor* NewTarget) const;

	virtual FGenericTeamId GetGenericTeamId() const override;
	
	// Overriding the default SetFocus focal point to use our overriden target location.
	// The target defines its own focal point which SetFocus then stares at.
	virtual FVector GetFocalPointOnActor(const AActor* Actor) const override;
	
	/** Perception event handling data for the Detection attribute. Detection dictates AI state */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection")
	TObjectPtr<UAIDetectionData> DetectionData;
	
	/** Returns the detection magnitude that the AI needs in order to go into search state.
	 * Used in multiple scenarios such as applying assist tag and detection level impulses
	 */
	UFUNCTION(BlueprintCallable, Category = "Detection")
	float GetSearchThreshold() const;

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
	
	void HandleSightSense(AActor* Actor, const FAIStimulus& Stimulus);
	void HandleHearingSense(AActor* Actor, const FAIStimulus& Stimulus) const;
	void HandleDamageSense(AActor* Actor, const FAIStimulus& Stimulus) const;
	void HandleTeamSense(AActor* Actor, const FAIStimulus& Stimulus) const;

	/** Update targets current state tag to input param tag */
	void UpdateTargetState(AActor* Target, FGameplayTag NewStateTag);
	
	void ProcessTargetSensed(AActor* TargetActor, const FGameplayTag ReactionTag, const FVector& StimulusLocation);
	
	void ProcessTargetLost(AActor* TargetActor, const FGameplayTag ReactionTag, const FAIStimulus&);
	
	/** Applies an amount of detection level to the AI instantly */
	void ApplyDetectionImpulse(AActor* InstigatorActor, const float DetectionAmount) const;
	
	/** Removes targets from memory if they have no active perception AND no remaining detection level */
	void PruneTargets();
	
private:
	bool bIsEvaluatingTargets = false;
	bool bEvaluationPending = false;
};
