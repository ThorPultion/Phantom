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
	void InitUI();
	
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void DestroyHUD();

protected:

	/** Widget containing the entire gameplay HUD */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCoreUserWidget> HUDClass;

	/** Widget containing the entire menu UI */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCoreUserWidget> MenuClass;

private:
	UPROPERTY()
	TObjectPtr<UCoreUserWidget> HUDWidget;
	
	UPROPERTY()
	TObjectPtr<UCoreUserWidget> MenuWidget;
	
};
