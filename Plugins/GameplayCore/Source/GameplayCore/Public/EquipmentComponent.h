// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

class UEquipmentDefinition;
class UCoreAbilitySystemComponent;
class AEquipmentBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAYCORE_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	/** Equip item defined in DataAsset field */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void EquipItem(UEquipmentDefinition* EquipmentDefinition, int32 SlotIndex = -1);

	/** Unequip and clean up equipped item */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void UnequipItem();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Equipment")
	void Server_UnequipItem();

	/** Add item to possible equippables */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void AddItemToLoadout(UEquipmentDefinition* NewItem);

	/** Equip item by slot index */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void EquipItemFromSlot(int32 SlotIndex);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Equipment")
	void Server_EquipItemFromSlot(int32 SlotIndex);

	/** Equip next weapon (wraps to start at end) */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void EquipNextItem();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Equipment")
	void Server_EquipNextItem();

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void CycleActiveItemAmmo(int32 Direction);

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void SetItemAmmoIndex(int32 AmmoIndex);

public:
	/** Gets the physical equipment actor currently spawned in the world */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	AEquipmentBase* GetCurrentItem() const { return CurrentItem; }

	/** Default loadout of equipment */
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<TObjectPtr<UEquipmentDefinition>> StartingEquipment;

protected:
	// The physically spawned weapon actor
	// Replicating weapon swap to local player for first person visual matching
	UPROPERTY(ReplicatedUsing = OnRep_CurrentItem)
	TObjectPtr<AEquipmentBase> CurrentItem;

	/** Currently used for weapon first person mesh attachment for local player */
	UFUNCTION()
	void OnRep_CurrentItem(AEquipmentBase* OldItem);

	// Tracks current equipment DataAsset for unequipping
	UPROPERTY()
	TObjectPtr<UEquipmentDefinition> CurrentEquipmentDefinition;

	// The items the player is "carrying", what can be equipped
	UPROPERTY(Replicated)
	TArray<TObjectPtr<UEquipmentDefinition>> Loadout;

	// Which slot in the Loadout array is currently spawned in the players hands?
	UPROPERTY(ReplicatedUsing = OnRep_CurrentSlotIndex)
	int32 CurrentSlotIndex = -1;

	UFUNCTION()
	void OnRep_CurrentSlotIndex();

	// Required for network replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Max number of carriable equipment
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	int32 MaxLoadoutSlots = 2;

	// Helper function to find the owners ASC
	UCoreAbilitySystemComponent* GetOwnerASC() const;
		
};
