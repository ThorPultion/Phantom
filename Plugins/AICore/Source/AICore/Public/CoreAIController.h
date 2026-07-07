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
};
