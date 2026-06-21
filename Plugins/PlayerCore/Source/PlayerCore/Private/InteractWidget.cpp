// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractWidget.h"
#include "Components/TextBlock.h"
#include "CorePlayerCharacter.h"

void UInteractWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Text should be hidden by default
	if (InteractText)
	{
		InteractText->SetVisibility(ESlateVisibility::Hidden);
	}

	// Binding to player interact target change delegate
	if (ACorePlayerCharacter* PlayerChar = Cast<ACorePlayerCharacter>(GetOwningPlayerPawn()))
	{
		PlayerChar->OnInteractionFocusChanged.AddDynamic(this, &UInteractWidget::UpdateInteractText);
	}
}

void UInteractWidget::UpdateInteractText(bool bIsLookingAtItem, const FText& PromptText)
{
	if (!InteractText) return;

	if (bIsLookingAtItem)
	{
		InteractText->SetText(PromptText);
		InteractText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractText->SetVisibility(ESlateVisibility::Hidden);
	}
}