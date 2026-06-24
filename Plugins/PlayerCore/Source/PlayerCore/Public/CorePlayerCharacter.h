// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "CoreCharacterBase.h"
#include "CorePlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;

// Broadcasting when the player looks at or looks away from an interactable
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionFocusChanged, bool, bIsLookingAtItem, const FText&, PromptText);

UCLASS()
class PLAYERCORE_API ACorePlayerCharacter : public ACoreCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACorePlayerCharacter();

	/** Server-side initialization hook */
	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	/** Client-side initialization hook */ 
	virtual void OnRep_PlayerState() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Overrides base class. Returns actual 1P mesh to the equipment system */
	virtual USkeletalMeshComponent* GetFirstPersonMesh() const override { return FirstPersonMesh; }

	/** Broadcasts when the player looks at or looks away from an interactable */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionFocusChanged OnInteractionFocusChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Initiate Gameplay Ability System */
	virtual void InitAbilitySystem() override;

	/** The flag to solve the client input race condition */ 
	bool bIsClientInitialized = false;

	/** A dedicated function to handle post GAS and input component setup */
	void TryInitializeLocalPlayer();

	/** List of input actions mapped to Gameplay Tags, used by the custom Input Component to trigger abilities */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UCoreInputConfig> InputConfigDataAsset;

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	/** Player Mapping Context */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Input Action for moving player */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	/** Input Action for player camera look */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	float DefaultCameraDistance = 0.0f;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	/** How far the players interaction can reach */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractTraceDistance = 500.0f;

	// A pointer to hold whatever object we are currently looking at
	UPROPERTY()
	TObjectPtr<AActor> FocusedInteractable;

	// Fires continuously to check what the interaction line trace hits
	void PerformInteractionCheck();

	/** Input Action for Interacting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	// Interaction through GameplayEvent to GA
	void Input_Interact(const FInputActionValue& Value);

	/** Input Action for selecting ammo by index */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SelectAmmoAction;

	// Ammo selection by index through GameplayEvent to GA
	void Input_SelectAmmo(const FInputActionValue& Value);

	/** Input Action for cycling between ammo */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> CycleAmmoAction;

	// Ammo cycling through GameplayEvent to GA
	void Input_CycleAmmo(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
