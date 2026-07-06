// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "CoreCharacterBase.generated.h"

class UAbilitySystemComponent;
class UCoreAttributeSet;
class UGASInitData;
struct FStreamableHandle;
class UEquipmentComponent;
class USkeletalMeshComponent;
struct FOnAttributeChangeData;
struct FGameplayTag;

UCLASS(Abstract)
class GAMEPLAYCORE_API ACoreCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACoreCharacterBase();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// IAbilitySystemInterface requirement
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UCoreAttributeSet* GetAttributeSet() const { return AttributeSet; }

	/** Public getter for our equipment manager */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	UEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }

	/** Virtual public getter so child classes can provide a 1P mesh if they have one */ 
	virtual USkeletalMeshComponent* GetFirstPersonMesh() const { return nullptr; }

protected:
	// Pointers are set by child classes, so the base class can just use them
	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UCoreAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TObjectPtr<UGASInitData> GASInitData;

	/** Manages equipment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	virtual void GrantStartingAbilities();

	virtual void SetupAttributes();

	virtual void GrantPassiveEffects();

	virtual void GrantStartingEquipment();

	// Called by child classes once their ASC is valid
	virtual void InitAbilitySystem();

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	// Callback function for when movement speed attribute changes
	virtual void OnMovementSpeedChanged(const FOnAttributeChangeData& Data);

	FDelegateHandle MovementSpeedChangedDelegateHandle;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsAiming;

	FDelegateHandle AimingTagDelegateHandle;

	void OnAimingTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
