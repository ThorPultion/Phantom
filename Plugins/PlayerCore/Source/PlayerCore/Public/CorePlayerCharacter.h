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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Initiate Gameplay Ability System */
	virtual void InitAbilitySystem() override;

	/** The flag to solve the client input race condition */ 
	bool bIsInputBound = false;

	/** A dedicated function to handle input binding safely */
	void BindASCInput();

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
	UInputAction* MoveAction;

	/** Input Action for player camera look */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Settings")
	float DefaultCameraDistance = 0.0f;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
