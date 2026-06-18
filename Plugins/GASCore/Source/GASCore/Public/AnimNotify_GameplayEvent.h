// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_GameplayEvent.generated.h"

/**
 * A custom Anim Notify that fires a Gameplay Event to the owning actor.
 * Used for triggering attack logic (like a hit trace) at an exact frame in an animation
 */
UCLASS(meta = (DisplayName = "Send Gameplay Event"))
class GASCORE_API UAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_GameplayEvent();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	/** Changes the name of the notify in the timeline to match the tag */ 
	virtual FString GetNotifyName_Implementation() const override;

protected:
	/** The tag we want to send */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Event")
	FGameplayTag EventTag;
};