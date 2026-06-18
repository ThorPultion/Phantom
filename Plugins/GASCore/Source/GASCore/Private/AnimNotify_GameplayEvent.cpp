// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_GameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

UAnimNotify_GameplayEvent::UAnimNotify_GameplayEvent()
{
#if WITH_EDITORONLY_DATA
	// Makes the notify block visually stand out (orange) in the animation timeline
	NotifyColor = FColor(255, 128, 0, 255);
#endif
}

void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		AActor* OwnerActor = MeshComp->GetOwner();

		// Ensure we actually assigned a tag in the animation editor
		if (EventTag.IsValid())
		{
			// Construct an empty payload. We can expand this struct later if we need 
			// to pass specific magnitudes or target data directly from the animation.
			FGameplayEventData Payload;
			Payload.EventTag = EventTag;
			Payload.Instigator = OwnerActor;

			// Broadcast the event to the actors Ability System Component
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, Payload);
		}
	}
}

FString UAnimNotify_GameplayEvent::GetNotifyName_Implementation() const
{
	// If a tag is set, display the tag name on the animation timeline instead of "AnimNotify_GameplayEvent"
	if (EventTag.IsValid())
	{
		return EventTag.ToString();
	}

	return Super::GetNotifyName_Implementation();
}