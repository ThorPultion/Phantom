// Fill out your copyright notice in the Description page of Project Settings.

#include "StateTreeTask_TurnToLocation.h"
#include "CoreAIController.h"
#include "StateTreeLinker.h"
#include "StateTreeExecutionContext.h"

FStateTreeTask_TurnToLocation::FStateTreeTask_TurnToLocation()
{
	bShouldCallTick = true; // We need this to update the rotation smoothly over frames
}

bool FStateTreeTask_TurnToLocation::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(AIControllerHandle);
	return true;
}

EStateTreeRunStatus FStateTreeTask_TurnToLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	ACoreAIController* AIController = Context.GetExternalDataPtr(AIControllerHandle);
    
	if (AIController)
	{
		// Telling the AI to lock onto this exact coordinate.
		// The engine will now automatically update the Control Rotation under the hood
		AIController->SetFocalPoint(InstanceData.TargetLocation, EAIFocusPriority::Gameplay);
	}
	// Return Running so the StateTree knows to start calling Tick()
	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus FStateTreeTask_TurnToLocation::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
    
	ACoreAIController* AIController = Context.GetExternalDataPtr(AIControllerHandle);
	if (!IsValid(AIController))
	{
		return EStateTreeRunStatus::Failed;
	}

	const APawn* ControlledPawn = AIController->GetPawn();
	if (!IsValid(ControlledPawn))
	{
		return EStateTreeRunStatus::Failed;
	}

	const FVector PawnLocation = ControlledPawn->GetActorLocation();
    
	// Flattening the Z axis so the AI doesnt tilt upward if the target is high up
	const FVector DirectionToTarget = (InstanceData.TargetLocation - PawnLocation).GetSafeNormal2D();
    
	if (DirectionToTarget.IsNearlyZero())
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		return EStateTreeRunStatus::Succeeded;
	}

	// Getting where the physical mesh is currently facing, flattened
	const FVector PawnForward = ControlledPawn->GetActorForwardVector().GetSafeNormal2D();

	// Using Dot Product to find the angle between current forward and target direction
	const float DotResult = FVector::DotProduct(PawnForward, DirectionToTarget);
    
	// Clamping to prevent NaN errors in Acos from floating point imprecision near 1.0 or -1.0
	const float ClampedDot = FMath::Clamp(DotResult, -1.0f, 1.0f);
	const float AngleDiff = FMath::RadiansToDegrees(FMath::Acos(ClampedDot));
    
	// When the AI has rotated within tolerance, clear focus and move on
	if (AngleDiff <= InstanceData.AngleTolerance)
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}