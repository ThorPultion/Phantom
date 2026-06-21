// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraModifier_Zoom.h"

bool UCameraModifier_Zoom::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
    Super::ModifyCamera(DeltaTime, InOutPOV);

    // Capture pre modifier FOV
    if (OriginalFOV == 0.0f)
    {
        OriginalFOV = InOutPOV.FOV;
        InterpolatedFOV = OriginalFOV;
    }

    // Interpolate FOV (zoom)
    InterpolatedFOV = FMath::FInterpTo(InterpolatedFOV, TargetFOV, DeltaTime, ZoomSpeed);
    InOutPOV.FOV = InterpolatedFOV;

    // If modifier effect is being reverted, and we are practically back at original FOV
    if (bIsReverting && FMath::IsNearlyEqual(InterpolatedFOV, TargetFOV, 0.1f))
    {
        if (CameraOwner)
        {
            // Remove this modifier from the camera manager
            CameraOwner->RemoveCameraModifier(this);
        }
    }

    return false;
}

void UCameraModifier_Zoom::ReverseModifier()
{
    // Reversing back to OriginalFOV
    bIsReverting = true;
    TargetFOV = OriginalFOV;
}