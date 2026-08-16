// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PhantomGameMode.generated.h"

class UCollectibleDropTable;

/**
 * 
 */
UCLASS()
class PHANTOM_API APhantomGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void StartPlay() override;
	
	/** The collectibles that should be spawned around the map */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collectible Spawning")
	TObjectPtr<UCollectibleDropTable> CollectibleDropTable;
	
	/** The rare collectibles that should be spawned around the map */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collectible Spawning")
	TObjectPtr<UCollectibleDropTable> RareCollectibleDropTable;
	
private:
	void InitCollectibles() const;
};
