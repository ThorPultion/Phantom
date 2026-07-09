// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CoreAIController.generated.h"

class UStateTreeAIComponent;
class UCoreAIPerceptionComponent;
struct FAIStimulus;

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

	/** The AIs current target (Looking, chasing, investigating, etc) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Memory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> CurrentTargetActor = nullptr;

	/** The AIs current movement goal */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Memory", meta = (AllowPrivateAccess = "true"))
	FVector CurrentTargetLocation = FVector::ZeroVector;

public:

	/** Set the AIs current target (Looking, chasing, investigating, etc) */
	void SetCurrentTargetActor(AActor* NewTarget) { CurrentTargetActor = NewTarget; }
	/** Get the AIs current target (Looking, chasing, investigating, etc) */
	AActor* GetCurrentTargetActor() const { return CurrentTargetActor; }

	/** Set the AIs current movement goal */
	void SetCurrentTargetLocation(const FVector& NewLocation) { CurrentTargetLocation = NewLocation; }
	/** Get the AIs current movement goal */
	FVector GetCurrentTargetLocation() const { return CurrentTargetLocation; }
};
