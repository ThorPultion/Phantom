// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemDefinition.h"
#include "CollectibleDefinition.generated.h"


UENUM(BlueprintType)
enum class ECollectibleBehavior : uint8
{
	Valuable    UMETA(DisplayName = "Add Valuable"),
	Ability     UMETA(DisplayName = "Grant Gameplay Ability System Effect")
};

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UCollectibleDefinition : public UItemDefinition
{
	GENERATED_BODY()
	
public:

	/** What kind of collectible this is */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collectible Logic")
	ECollectibleBehavior BehaviorType = ECollectibleBehavior::Valuable;

	/** The worth of the item in gold */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collectible Logic",
		meta = (EditCondition = "BehaviorType == ECollectibleBehavior::Valuable", EditConditionHides))
	float WorthInGold;

	/** Gameplay effect thats applied on pickup */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collectible Logic",
		meta = (EditCondition = "BehaviorType == ECollectibleBehavior::Ability", EditConditionHides))
	TSubclassOf<class UGameplayEffect> EffectToApply;
	
	void CollectCollectible(AActor* Interactor) const;
};
