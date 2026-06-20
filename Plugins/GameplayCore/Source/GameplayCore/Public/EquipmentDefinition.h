// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EquipmentDefinition.generated.h"

class AWeaponBase;
class UCoreAbilitySet;
/**
 * 
 */
UCLASS(BlueprintType)
class GAMEPLAYCORE_API UEquipmentDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
    // The actual actor to spawn (AWeaponBase Blueprint)
    UPROPERTY(EditDefaultsOnly, Category = "Equipment")
    TSubclassOf<AWeaponBase> WeaponActorClass;

    // The socket on the 1P/3P mesh to attach to
    UPROPERTY(EditDefaultsOnly, Category = "Equipment")
    FName AttachmentSocket;

    // The GAS abilities to grant when equipped
    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TObjectPtr<UCoreAbilitySet> GrantedAbilitySet;
};
