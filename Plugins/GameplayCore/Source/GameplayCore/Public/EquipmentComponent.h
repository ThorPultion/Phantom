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

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void AddItemToLoadout(UEquipmentDefinition* NewItem);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipItemFromSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipNextWeapon();

protected:
	// The physically spawned weapon actor
	// Replicate weapon, and run a function when it arrives
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	TObjectPtr<AWeaponBase> CurrentWeapon;

	UFUNCTION()
	void OnRep_CurrentWeapon(AWeaponBase* OldWeapon);

	UPROPERTY()
	TObjectPtr<UEquipmentDefinition> CurrentEquipmentDefinition;

	// The items the player is currently carrying in their invisible backpack
	UPROPERTY(Replicated)
	TArray<TObjectPtr<UEquipmentDefinition>> Loadout;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<TObjectPtr<UEquipmentDefinition>> DefaultLoadout;

	// Which slot in the Loadout array is currently spawned in the players hands?
	UPROPERTY(ReplicatedUsing = OnRep_CurrentSlotIndex)
	int32 CurrentSlotIndex = -1;

	UFUNCTION()
	void OnRep_CurrentSlotIndex();

	// Required for network replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Max number of equipment they can carry
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	int32 MaxLoadoutSlots = 2;

	// Helper function to find the owner's ASC
	UCoreAbilitySystemComponent* GetOwnerASC() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
