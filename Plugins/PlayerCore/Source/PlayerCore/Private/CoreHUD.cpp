// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreHUD.h"
#include "CoreUserWidget.h"

void ACoreHUD::InitUI()
{
	// Gameplay HUD init
	// Only spawning if we have a class and havent spawned it yet
	if (HUDClass && HUDWidget == nullptr)
	{
		// Creating the widget and storing a reference
		HUDWidget = CreateWidget<UCoreUserWidget>(GetWorld(), HUDClass);

		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
	
	// Menu UI init
	if (MenuClass && MenuWidget == nullptr)
	{
		MenuWidget = CreateWidget<UCoreUserWidget>(GetWorld(), MenuClass);

		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
		}
	}
}

void ACoreHUD::DestroyUI()
{
	if (IsValid(HUDWidget))
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}
	
	if (IsValid(MenuWidget))
    {
    	MenuWidget->RemoveFromParent();
		MenuWidget = nullptr;
    }
}