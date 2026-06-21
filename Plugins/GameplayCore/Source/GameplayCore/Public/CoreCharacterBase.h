// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "CoreCharacterBase.generated.h"

class UAbilitySystemComponent;
class UCoreAttributeSet;
class UCoreAbilitySet;
struct FStreamableHandle;
class UEquipmentComponent;
class USkeletalMeshComponent;

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

	/** Virtual public getter for players currently focused interactable object */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual AActor* GetFocusedInteractable() const { return nullptr; }

protected:
	// Pointers are set by child classes, so the base class can just use them
	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UCoreAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TObjectPtr<UCoreAbilitySet> StartingAbilities;

	/** Manages equipment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	virtual void GiveStartingAbilities();

	// Called by child classes once their ASC is valid
	virtual void InitAbilitySystem();

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
};
