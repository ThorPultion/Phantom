// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "EquipmentDefinition.generated.h"

class AWeaponBase;
class UCoreAbilitySet;
/**
 * 
 */
UCLASS(BlueprintType)
class GAMEPLAYCORE_API UEquipmentDefinition : public UItemDefinition
{
	GENERATED_BODY()
	
public:
    // Maybe should make actor to spawn generic Actor instead?
    // or make AEquipmentBase separately or as weaponbases parent

    /** Weapon actor to spawn */
    UPROPERTY(EditDefaultsOnly, Category = "Equipment")
    TSubclassOf<AWeaponBase> WeaponActorClass;

    /** The socket on the 1P/3P mesh of the weapon to attach to */
    UPROPERTY(EditDefaultsOnly, Category = "Equipment")
    FName AttachmentSocket;

    /** The GAS abilities to grant when equipped */ 
    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TObjectPtr<UCoreAbilitySet> GrantedAbilitySet;
};
