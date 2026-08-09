// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreUserWidget.h"
#include "StatusWidget.generated.h"

struct FOnAttributeChangeData;
/**
 * 
 */
UCLASS()
class PLAYERCORE_API UStatusWidget : public UCoreUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	/** Update UI based on new health percent */
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnHealthUpdated(const float NewHealthPercent);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	float MaxHealth;
	
private:
	float CurrentHealth = 0.f;
	
	// Cant bind a BlueprintImplementableEvent directly to a c++ delegate so we have a middle handler
	void HandleHealthChanged(const FOnAttributeChangeData& Data);
	
	void HandleMaxHealthChanged(const FOnAttributeChangeData& Data);
	
	void RecalculateHealthPercent();
};
