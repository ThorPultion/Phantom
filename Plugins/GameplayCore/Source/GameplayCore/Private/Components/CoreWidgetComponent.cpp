// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CoreWidgetComponent.h"
#include "Interfaces/ContextInitializable.h"

void UCoreWidgetComponent::InitWidget()
{
	Super::InitWidget();

	if (UUserWidget* UserWidget = GetUserWidgetObject())
	{
		if (UserWidget->Implements<UContextInitializable>())
		{
			IContextInitializable::Execute_OnContextInitialized(UserWidget, GetOwner());
		}
	}
}