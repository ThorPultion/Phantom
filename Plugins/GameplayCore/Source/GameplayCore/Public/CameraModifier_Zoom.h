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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float TargetFOV = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float ZoomSpeed = 10.0f;

protected:

	// Add this to remember our state
	UPROPERTY()
	float CurrentFOV = 0.0f;

	// Remembers where we started
	UPROPERTY()
	float DefaultFOV = 0.0f;

	// Are we currently zooming out?
	UPROPERTY()
	bool bIsReverting = false; 
};
