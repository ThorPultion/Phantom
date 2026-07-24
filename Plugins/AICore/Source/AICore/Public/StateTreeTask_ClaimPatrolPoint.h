// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeTask_ClaimPatrolPoint.generated.h"

class UEnvQuery;
class APatrolPoint;
class AAIController;

/** Thread safe bridge to hold data between the background EQS thread and the StateTree Game Thread. */
struct FPatrolQueryBridge
{
	bool bIsFinished = false;
	bool bWasSuccessful = false;
	TWeakObjectPtr<APatrolPoint> ClaimedPoint = nullptr;
	FVector PointLocation = FVector::ZeroVector;
};

/** Instance data for the Claim Patrol Point task.
* Exposes parameters to the StateTree editor UI. */
USTRUCT()
struct FStateTreeTask_ClaimPatrolPoint_InstanceData
{
	GENERATED_BODY()

	/** The EQS Query asset to run (e.g., EQS_FindPatrolPoint) */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<UEnvQuery> FindPatrolPointQuery = nullptr;

	/** Input: The AI Controller executing this task */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> AIController = nullptr;

	/** Maximum time (in seconds) to wait for the query before timing out and failing */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float TimeoutDuration = 3.0f;
	
	// --- OUTPUTS ---

	/** The claimed patrol point actor (used for releasing later) */
	UPROPERTY(EditAnywhere, Category = "Output")
	TObjectPtr<APatrolPoint> OutPatrolPoint = nullptr;

	/** The exact world location to move to */
	UPROPERTY(EditAnywhere, Category = "Output")
	FVector OutLocation = FVector::ZeroVector;

	// --- INTERNAL RUNTIME DATA ---
	/** Tracks elapsed time to handle async timeouts */
	float ElapsedTime = 0.0f;

	// Non-property C++ shared pointer to safely bridge the async callback
	TSharedPtr<FPatrolQueryBridge> QueryBridge = nullptr;
};

/** Executes an EQS query to find available patrol points, claims the highest scoring
 * unclaimed point, and outputs it */
USTRUCT(meta = (DisplayName = "Find And Claim Next Patrol Point", Category = "Patrol"))
struct AICORE_API FStateTreeTask_ClaimPatrolPoint : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FStateTreeTask_ClaimPatrolPoint_InstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
