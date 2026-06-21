// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "PickupBase.generated.h"

class UItemDefinition;
class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class GAMEPLAYCORE_API APickupBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupBase();

	// Interactable interface functions
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractText_Implementation() override;

protected:

    virtual void OnConstruction(const FTransform& Transform) override;

    /** The objects definition */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
    TObjectPtr<UItemDefinition> ItemData;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> RootComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> PickupMesh;

};
