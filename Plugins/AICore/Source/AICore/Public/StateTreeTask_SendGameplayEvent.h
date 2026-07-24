// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "GameplayTagContainer.h"
#include "StateTreeTask_SendGameplayEvent.generated.h"

/**
 * 
 */
USTRUCT()
struct FStateTreeTask_SendGameplayEventInstanceData
{
	GENERATED_BODY()

	/** The actor that activates the ability */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor = nullptr;

	/** If the ability needs a target to be passed in */
	UPROPERTY(EditAnywhere, Category = "Input", meta = (Optional))
	TObjectPtr<AActor> PayloadTarget = nullptr;

	/** GameplayAbility trigger tag */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTag EventTag;

	/** Should this node pause until ability completion? */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bWaitForCompletion = false;

	/** The tag that is applied by the ability which we wait to disappear before resuming StateTree */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (EditCondition = "bWaitForCompletion"))
	FGameplayTag InProgressTag;
};

USTRUCT(meta = (DisplayName = "Send GAS Event", Category = "GAS"))
struct AICORE_API FStateTreeTask_SendGameplayEvent : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FStateTreeTask_SendGameplayEventInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	// NEW: We need Tick to monitor the tag
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};