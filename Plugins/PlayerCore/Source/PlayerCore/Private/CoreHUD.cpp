// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreHUD.h"
#include "CoreUserWidget.h"

void ACoreHUD::InitHUD()
{
	// Only spawn if we have a class and havent spawned it yet
	if (MasterHUDClass && MasterHUDWidget == nullptr)
	{
		// Create the widget and store the reference
		MasterHUDWidget = CreateWidget<UCoreUserWidget>(GetWorld(), MasterHUDClass);

		if (MasterHUDWidget)
		{
			MasterHUDWidget->AddToViewport();
		}
	}
}