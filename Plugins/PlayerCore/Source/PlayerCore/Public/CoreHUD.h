// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CoreHUD.generated.h"

class UCoreUserWidget;

/**
 * 
 */
UCLASS()
class PLAYERCORE_API ACoreHUD : public AHUD
{
	GENERATED_BODY()

public:
	// The Character will call this when GAS is ready
	void InitHUD();

protected:

	// The class we will select in the Blueprint Editor (e.g. WBP_MasterHUD)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCoreUserWidget> MasterHUDClass;

private:
	// A pointer to the actual widget once it is created
	UPROPERTY()
	TObjectPtr<UCoreUserWidget> MasterHUDWidget;
};
