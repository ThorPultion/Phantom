// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraModifier_Zoom.h"

bool UCameraModifier_Zoom::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
    Super::ModifyCamera(DeltaTime, InOutPOV);

    // 1. On the very first frame, capture the camera's original FOV
    if (DefaultFOV == 0.0f)
    {
        DefaultFOV = InOutPOV.FOV;
        CurrentFOV = DefaultFOV;
    }

    // 2. Interpolate
    CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, ZoomSpeed);
    InOutPOV.FOV = CurrentFOV;

    // 3. Self-Destruct Logic: If we are zooming out, and we are practically back at the default FOV...
    if (bIsReverting && FMath::IsNearlyEqual(CurrentFOV, TargetFOV, 0.5f))
    {
        if (CameraOwner)
        {
            // Remove ourselves from the camera manager
            CameraOwner->RemoveCameraModifier(this);
        }
    }

    return false;
}

void UCameraModifier_Zoom::ReverseModifier()
{
    // Tell the modifier to start heading back to where it started
    bIsReverting = true;
    TargetFOV = DefaultFOV;
}