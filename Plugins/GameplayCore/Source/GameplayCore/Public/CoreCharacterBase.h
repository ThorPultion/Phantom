// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/Perceivable.h"
#include "GenericTeamAgentInterface.h"
#include "Data/AIReactionData.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISightTargetInterface.h"
#include "CoreCharacterBase.generated.h"

class UAbilitySystemComponent;
class UCoreAttributeSet;
class UGASInitData;
struct FStreamableHandle;
class UEquipmentComponent;
class USkeletalMeshComponent;
struct FOnAttributeChangeData;
struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilitySystemInitialized, UAbilitySystemComponent*, ASC);
UCLASS(Abstract)
class GAMEPLAYCORE_API ACoreCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface, public IPerceivable, public IAISightTargetInterface
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
	
	/** To be overriden by child classes. Returns how lit the character is */
	virtual float GetVisibilityModifier_Implementation() override;
	
	virtual float GetInvisibleThreshold_Implementation() override { return 0.15f; }
	
	/** IAISightTargetInterface override. This is how perception system checks if this is still visible to observer */
virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(
    const FCanBeSeenFromContext& Context,
    FVector& OutSeenLocation,
    int32& OutNumberOfLoSChecksPerformed,
    int32& OutNumberOfAsyncLosCheckRequested,
    float& OutSightStrength,
    int32* UserData = nullptr,
    const FOnPendingVisibilityQueryProcessedDelegate* Delegate = nullptr) override;

protected:
	// Pointers are set by child classes, so the base class can just use them
	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UCoreAttributeSet> AttributeSet;

	/** Data used to initialize attributes, abilities and effects */
	UPROPERTY(EditAnywhere, Category = "GAS")
	TObjectPtr<UGASInitData> GASInitData;

	/** Delegate for external systems to bind to that need GAS info at startup. Fetch ASC, if ASC invalid, bind to this!*/
	UPROPERTY(BlueprintAssignable, Category = "GAS")
	FOnAbilitySystemInitialized OnAbilitySystemInitialized;

	/** Manages equipment */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	virtual void GrantStartingAbilities();

	virtual void SetupAttributes();

	virtual void GrantPassiveEffects();

	virtual void GrantStartingEquipment();

	// Called by child classes once their ASC is valid
	virtual void InitAbilitySystem();

	void BindAbilitySystemDelegates();

	// Callback function for when movement speed attribute changes
	virtual void OnMovementSpeedChanged(const FOnAttributeChangeData& Data);

	FDelegateHandle MovementSpeedChangedDelegateHandle;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsAiming;

	FDelegateHandle AimingTagDelegateHandle;

	void OnAimingTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	FDelegateHandle DeadTagDelegateHandle;
	
	virtual void OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	/** The point at which SetFocus will look at */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	FName SetFocusTargetSocket = "set_focus_socket";
	
	/** Assign team in blueprints, 0 = Players, 1 = Guards, 2 = Monsters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FGenericTeamId TeamID;

public:
	/** What team this character is on */
	virtual FGenericTeamId GetGenericTeamId() const override;
	
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	
	/** Giving AI reaction tags for when this character is perceived */
	virtual FGameplayTag GetPerceptionTag_Implementation(ETeamAttitude::Type ObserverAttitude, const FAIStimulus& Stimulus) override;
	
	/** Overriding default target location for AI SetFocus */
	virtual FVector GetTargetLocation(AActor* RequestedBy = nullptr) const override;
	
	/** What reaction data this character gives to AI */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perception")
	TObjectPtr<UAIReactionData> ReactionData;
	
};
