// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "StateTreeLineOfSightCondition.generated.h"

class AAIController;

/**
 * Inputs required for the LoS condition. 
 * You will bind these in the StateTree Editor to your AI Controller and Target Actor.
 */
USTRUCT()
struct FStateTreeLineOfSightConditionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor = nullptr;
};

/**
 * Checks if the AI Controllers perception component has an active sight stimulus for target actor
 */
USTRUCT(DisplayName = "Has Line of Sight (Perception)")
struct AICORE_API FStateTreeLineOfSightCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeLineOfSightConditionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
