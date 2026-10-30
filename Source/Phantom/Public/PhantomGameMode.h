// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PhantomGameMode.generated.h"

class UItemDropTable;

/**
 * 
 */
UCLASS()
class PHANTOM_API APhantomGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	APhantomGameMode();
	
	virtual void StartPlay() override;
	
	/** The collectibles that should be spawned around the map */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collectible Spawning")
	TObjectPtr<UItemDropTable> CollectibleDropTable;
	
	/** The rare collectibles that should be spawned around the map */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collectible Spawning")
	TObjectPtr<UItemDropTable> RareCollectibleDropTable;
	
	/** The equipment that should be spawned around the map */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Spawning")
	TObjectPtr<UItemDropTable> EquipmentDropTable;
	
	/** The ammo that should be spawned around the map */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo Spawning")
	TObjectPtr<UItemDropTable> AmmoDropTable;
	
private:
	void InitCollectiblePickups() const;
	void InitEquipmentPickups() const;
	void InitAmmoPickups() const;
};
