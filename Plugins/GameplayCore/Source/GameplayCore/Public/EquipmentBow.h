// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentBase.h"
#include "ProjectileProvider.h"
#include "AmmoCycler.h"
#include "EquipmentBow.generated.h"

class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API AEquipmentBow : public AEquipmentBase, public IProjectileProvider, public IAmmoCycler
{
	GENERATED_BODY()

public:
	AEquipmentBow();

	// ProjectileProvider Interface

	/** Gets active projectile class */
	virtual TSubclassOf<AActor> GetCurrentProjectileClass_Implementation() const override;

	/** Gets projectile spawn position */
	virtual FTransform GetProjectileSpawnTransform_Implementation() const override;

	// AmmoCycler Interface
	virtual void CycleAmmo_Implementation(int32 Direction) override;
	virtual void SetAmmoIndex_Implementation(int32 SpecificIndex) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** The dummy 1p mesh representing the nocked arrow before it is fired */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> FirstPersonLoadedArrowMesh;

	/** The dummy 3p mesh representing the nocked arrow before it is fired */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> ThirdPersonLoadedArrowMesh;

	/** The types of arrows this bow can fire. Order matters for cycling! */
	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	TArray<TSubclassOf<AActor>> AvailableArrowClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Socket")
	FName ShootingPointSocket;

	/** RepNotify so other clients know to update their visuals */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentArrowIndex)
	int32 CurrentArrowIndex;

	/** Called on clients when the server updates the arrow index */
	UFUNCTION()
	void OnRep_CurrentArrowIndex();

	/** * Blueprint event to handle the visual swap of the LoadedArrowMesh.
	 * This is triggered locally upon prediction, and on other clients via the RepNotify.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ammo")
	void OnArrowVisualsChanged(int32 NewIndex);
};