// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthWidget.h"
#include "CorePlayerCharacter.h"

void UStealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Our HUD init is ASC race condition safe, so we can initialize here safely
	if (ACorePlayerCharacter* PlayerChar = Cast<ACorePlayerCharacter>(GetOwningPlayerPawn()))
	{
		// Binding to lit value changed delegate
		PlayerChar->OnPlayerVisibilityChanged.AddDynamic(this, &UStealthWidget::HandleVisibilityChanged);

		// Initial UI update
		HandleVisibilityChanged(PlayerChar->Execute_GetVisibilityModifier(PlayerChar));
	}
}

void UStealthWidget::HandleVisibilityChanged(const float NewVisibility)
{
	// UI update
	OnVisibilityUpdated(NewVisibility);
}