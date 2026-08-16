// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectibleSpawnPoint.generated.h"

class UArrowComponent;

UCLASS()
class GAMEPLAYCORE_API ACollectibleSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectibleSpawnPoint();

	virtual void PostInitializeComponents() override;

	/** Is this spawn point designated to rare collectibles only? */
	UPROPERTY(EditDefaultsOnly, Category = "Rarity")
	bool bIsRare = false;
	
protected:
	// Editor only indicators for level placements
#if WITH_EDITORONLY_DATA
	/** Points the direction the AI should look if we care about doing that */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UArrowComponent> FacingDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> EditorIndicator;
#endif
};
