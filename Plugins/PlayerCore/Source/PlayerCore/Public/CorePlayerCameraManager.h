// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "CorePlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERCORE_API ACorePlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	ACorePlayerCameraManager();
	
	UFUNCTION(BlueprintCallable, Category = "Camera Tracking")
	void StartTrackingComponent(USceneComponent* TargetComponent, const FName SocketName = NAME_None);
	
	void StopTrackingComponent();
	
protected:
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Camera Tracking")
	float TrackingSpeed = 5.f;
	
private:
	
	UPROPERTY()
	TObjectPtr<USceneComponent> TrackingComponent;
	
	FName TrackingSocketName;
	
	FVector LockedCameraLocation;
	
	FQuat TrackingQuat;
	
	void TrackTargetComponent(FTViewTarget& OutVT, float DeltaTime);
};
