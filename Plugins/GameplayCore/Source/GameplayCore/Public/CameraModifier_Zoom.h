// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "CameraModifier_Zoom.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UCameraModifier_Zoom : public UCameraModifier
{
	GENERATED_BODY()
	

public:
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

public:
	void ReverseModifier();

	/** Zoom amount in FOV */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float TargetFOV = 45.0f;

	/** Speed of zoom interpolation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float ZoomSpeed = 10.0f;

protected:

	// Tracks the progress of the zooms interpolation
	float InterpolatedFOV = 0.0f;

	// Storing pre modifier FOV
	float OriginalFOV = 0.0f;

	// Are we currently zooming out?
	bool bIsReverting = false; 
};
