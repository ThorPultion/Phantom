// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreUserWidget.h"
#include "InteractWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PLAYERCORE_API UInteractWidget : public UCoreUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractText;

	UFUNCTION()
	void UpdateInteractText(bool bIsLookingAtItem, const FText& PromptText);
};
