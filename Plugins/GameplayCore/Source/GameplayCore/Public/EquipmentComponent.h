// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

class UEquipmentDefinition;
class UCoreAbilitySystemComponent;
class UCoreAbilitySet;
class AWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAYCORE_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipItem(UEquipmentDefinition* WeaponData);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnequipItem();

protected:
	// The physically spawned weapon actor
	UPROPERTY()
	TObjectPtr<AWeaponBase> CurrentWeapon;

	UPROPERTY()
	TObjectPtr<UEquipmentDefinition> CurrentEquipmentDefinition;

	// Helper function to find the owner's ASC
	UCoreAbilitySystemComponent* GetOwnerASC() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
