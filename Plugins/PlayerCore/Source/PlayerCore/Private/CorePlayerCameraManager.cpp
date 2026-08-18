// Fill out your copyright notice in the Description page of Project Settings.


#include "CorePlayerCameraManager.h"

ACorePlayerCameraManager::ACorePlayerCameraManager()
{
	ViewPitchMin = -72.f;
}

void ACorePlayerCameraManager::StartTrackingComponent(USceneComponent* TargetComponent, const FName SocketName)
{
	if (!TargetComponent) return;

	TrackingComponent = TargetComponent;
	TrackingSocketName = SocketName;
	
	LockedCameraLocation = GetCameraLocation();
	TrackingQuat = GetCameraRotation().Quaternion();
}

void ACorePlayerCameraManager::StopTrackingComponent()
{
	TrackingComponent = nullptr;
	TrackingSocketName = NAME_None;
}

void ACorePlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (IsValid(TrackingComponent))
	{
		TrackTargetComponent(OutVT, DeltaTime);
	}
}

void ACorePlayerCameraManager::TrackTargetComponent(FTViewTarget& OutVT, const float DeltaTime)
{
	OutVT.POV.Location = LockedCameraLocation;

	// If TrackingSocketName is NAME_None, it will automatically return root world location
	const FVector LookAtTargetLocation = TrackingComponent->GetSocketLocation(TrackingSocketName);

	const FVector DirectionToTarget = (LookAtTargetLocation - OutVT.POV.Location).GetSafeNormal();
	const FQuat TargetLookAtQuat = FRotationMatrix::MakeFromX(DirectionToTarget).ToQuat();
	
	const float Alpha = FMath::Clamp(DeltaTime * 5.0f, 0.f, 1.f);
	TrackingQuat = FQuat::Slerp(TrackingQuat, TargetLookAtQuat, Alpha);

	// Applying rotation
	OutVT.POV.Rotation = TrackingQuat.Rotator();
}
