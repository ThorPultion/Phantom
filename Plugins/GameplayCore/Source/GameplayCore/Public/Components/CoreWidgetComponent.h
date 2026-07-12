// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "CoreWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UCoreWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	// Native Unreal engine function that fires after the slate widget is created
	virtual void InitWidget() override;
};
