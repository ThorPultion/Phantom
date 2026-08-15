// Fill out your copyright notice in the Description page of Project Settings.


#include "CorePlayerController.h"
#include "CorePlayerCameraManager.h"

ACorePlayerController::ACorePlayerController()
{
	PlayerCameraManagerClass = ACorePlayerCameraManager::StaticClass();
}