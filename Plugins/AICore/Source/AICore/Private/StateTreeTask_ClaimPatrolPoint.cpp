// Fill out your copyright notice in the Description page of Project Settings.


#include "StateTreeTask_ClaimPatrolPoint.h"
#include "StateTreeExecutionContext.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "AIController.h"
#include "PatrolPoint.h"

EStateTreeRunStatus FStateTreeTask_ClaimPatrolPoint::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
    
    if (IsValid(InstanceData.ExistingPatrolPoint))
    {
        // Copy the existing point and its location directly to our outputs
        InstanceData.OutPatrolPoint = InstanceData.ExistingPatrolPoint;
        InstanceData.OutLocation = InstanceData.ExistingPatrolPoint->GetActorLocation();

        // Return immediately and dont run the rest of the task
        return EStateTreeRunStatus::Succeeded;
    }

    if (!InstanceData.AIController || !InstanceData.FindPatrolPointQuery)
    {
        return EStateTreeRunStatus::Failed;
    }

    InstanceData.ElapsedTime = 0.0f;
    InstanceData.OutPatrolPoint = nullptr;
    InstanceData.OutLocation = FVector::ZeroVector;

    // Creating the thread safe bridge and create a Weak Pointer for the lambda
    InstanceData.QueryBridge = MakeShared<FPatrolQueryBridge>();
    TWeakPtr<FPatrolQueryBridge> WeakBridge = InstanceData.QueryBridge;
    TWeakObjectPtr<AAIController> WeakController = InstanceData.AIController;

    // Executing the EQS Request
    FEnvQueryRequest Request(InstanceData.FindPatrolPointQuery, InstanceData.AIController);
    Request.Execute(EEnvQueryRunMode::RandomBest25Pct, FQueryFinishedSignature::CreateLambda(
        [WeakBridge, WeakController](const TSharedPtr<FEnvQueryResult>& Result)
        {
            // Try to pin the bridge. If the StateTree already transitioned away and destroyed our Task data,
            // this will safely return nullptr and prevent any memory crashes!
            const TSharedPtr<FPatrolQueryBridge> Bridge = WeakBridge.Pin();
            if (!Bridge.IsValid() || !WeakController.IsValid() || !Result.IsValid() || !Result->IsSuccessful())
            {
                if (Bridge.IsValid()) { Bridge->bIsFinished = true; }
                return;
            }

            TArray<AActor*> CandidateActors;
            Result->GetAllAsActors(CandidateActors);

            for (AActor* Actor : CandidateActors)
            {
                if (APatrolPoint* Point = Cast<APatrolPoint>(Actor))
                {
                    // Ensure no other AI claimed it while querying
                    if (!Point->IsClaimed())
                    {
                        Point->Claim(WeakController.Get());
                        
                        // Write results to our safe bridge
                        Bridge->ClaimedPoint = Point;
                        Bridge->PointLocation = Point->GetActorLocation();
                        Bridge->bWasSuccessful = true;
                        break;
                    }
                }
            }
            
            Bridge->bIsFinished = true;
        }));

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FStateTreeTask_ClaimPatrolPoint::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    // Timeout protection
    InstanceData.ElapsedTime += DeltaTime;
    if (InstanceData.ElapsedTime >= InstanceData.TimeoutDuration)
    {
        return EStateTreeRunStatus::Failed;
    }

    // Checking if the background EQS thread has finished writing to our bridge
    if (InstanceData.QueryBridge && InstanceData.QueryBridge->bIsFinished)
    {
        if (InstanceData.QueryBridge->bWasSuccessful && InstanceData.QueryBridge->ClaimedPoint.IsValid())
        {
            // Copy the data from the bridge to the StateTree Output Parameters
            InstanceData.OutPatrolPoint = InstanceData.QueryBridge->ClaimedPoint.Get();
            InstanceData.OutLocation = InstanceData.QueryBridge->PointLocation;
            return EStateTreeRunStatus::Succeeded;
        }
        
        // Query finished, but all points were taken or unreachable
        return EStateTreeRunStatus::Failed;
    }

    return EStateTreeRunStatus::Running;
}

void FStateTreeTask_ClaimPatrolPoint::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    // Cleanup: If this state aborted, release the point
    if (Transition.ChangeType != EStateTreeStateChangeType::Changed)
    {
        if (InstanceData.OutPatrolPoint && InstanceData.OutPatrolPoint->IsClaimed())
        {
            InstanceData.OutPatrolPoint->Release();
            InstanceData.OutPatrolPoint = nullptr;
        }
    }
}