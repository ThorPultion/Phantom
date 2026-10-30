// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquipmentSpawnPoint.generated.h"

class UArrowComponent;

UCLASS()
class GAMEPLAYCORE_API AEquipmentSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquipmentSpawnPoint();

	virtual void PostInitializeComponents() override;

protected:
	// Editor only indicators for level placements
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UArrowComponent> FacingDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> EditorIndicator;
#endif
};
