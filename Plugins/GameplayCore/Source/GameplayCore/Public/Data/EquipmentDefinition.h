// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "EquipmentDefinition.generated.h"

class AEquipmentBase;
class UCoreAbilitySet;
/**
 * 
 */
UCLASS(BlueprintType)
class GAMEPLAYCORE_API UEquipmentDefinition : public UItemDefinition
{
	GENERATED_BODY()
	
public:

    /** Equipment actor to spawn */
    UPROPERTY(EditDefaultsOnly, Category = "Equipment")
    TSubclassOf<AEquipmentBase> EquipmentActorClass;

    /** The GAS abilities to grant when equipped */ 
    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TObjectPtr<UCoreAbilitySet> GrantedAbilitySet;
};
