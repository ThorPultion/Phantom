// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "GameplayTagContainer.h"
#include "CoreAIStateEvaluator.generated.h"

USTRUCT()
struct FCoreAIStateEvaluatorInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Output")
	FGameplayTagContainer ActiveStateTags;
};

USTRUCT(meta = (DisplayName = "Core AI State Evaluator", Category = "Core AI"))
struct FCoreAIStateEvaluator : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FCoreAIStateEvaluatorInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};