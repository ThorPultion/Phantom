// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "CorePlayerState.generated.h"

class UCoreAbilitySystemComponent;
class UCoreAttributeSet;

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, float, NewGoldAmount);
UCLASS()
class PLAYERCORE_API ACorePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACorePlayerState();

	// IAbilitySystemInterface implementation
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UCoreAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	void AddGold(const float Amount);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintAssignable, Category = "Collectibles")
	FOnGoldChanged OnGoldChangedDelegate;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UCoreAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UCoreAttributeSet> AttributeSet;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles", ReplicatedUsing = OnRep_GatheredGold)
	float GatheredGold;
	
	UFUNCTION()
	void OnRep_GatheredGold(float OldGold) const;
};
