// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreUserWidget.h"
#include "StealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERCORE_API UStealthWidget : public UCoreUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	/** Update UI based on new visibility value */
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnVisibilityUpdated(const float VisibilityLevel);
	
private:
	// Cant bind a BlueprintImplementableEvent directly to a c++ delegate so we have a middle handler
	UFUNCTION()
	void HandleVisibilityChanged(const float NewVisibility);
};
