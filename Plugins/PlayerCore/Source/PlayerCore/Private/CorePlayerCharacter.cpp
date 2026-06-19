// Fill out your copyright notice in the Description page of Project Settings.


#include "CorePlayerCharacter.h"
#include "CorePlayerState.h"
#include "CoreAbilitySystemComponent.h"
#include "CoreInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GASCoreTags.h"

// Sets default values
ACorePlayerCharacter::ACorePlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = DefaultCameraDistance; // Distance from character
	// IMPORTANT! This connects Look input to the camera boom
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// Attach the camera to the end of the boom
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// The boom handles the rotation, the camera just goes along for the ride
	Camera->bUsePawnControlRotation = false;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->SetCastShadow(false);

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetCastHiddenShadow(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

// Called when the game starts or when spawned
void ACorePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ACorePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

void ACorePlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Running on the Client when the Player State replicates down
	InitAbilitySystem();

	// CLIENT INPUT RACE FIX:
	// Attempt to bind input now that the ASC is ready
	BindASCInput();
}

void ACorePlayerCharacter::InitAbilitySystem()
{
	ACorePlayerState* PS = GetPlayerState<ACorePlayerState>();
	if (PS)
	{
		// Assigning AbilitySystemComponent and AttributeSet for base class
		AbilitySystemComponent = PS->GetAbilitySystemComponent();

		AttributeSet = PS->GetAttributeSet();

		if (AbilitySystemComponent)
		{
			// Connects the GAS framework: Owner is PlayerState, Avatar is this physical body
			AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		}
	}
}

void ACorePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Get the Local Player Subsystem for Enhanced Input
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// Clear any existing contexts (useful if respawning) and add the default context
			InputSubsystem->ClearAllMappings();

			if (DefaultMappingContext)
			{
				// 0 is the priority. Higher numbers override lower numbers
				InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// Binding continuous inputs
	if (UEnhancedInputComponent* IC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind the Move action (Triggered fires every frame the input is held)
		IC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

		// Bind the Look action
		IC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	}

	// CLIENT INPUT RACE FIX: 
	// Attempt to bind input now that the Input Component is ready
	BindASCInput();
}

void ACorePlayerCharacter::BindASCInput()
{
	// If we already bound it, bail out
	if (bIsInputBound)
	{
		return;
	}

	// We can only bind if BOTH the ASC and the InputComponent are ready
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC && InputComponent)
	{
		UCoreInputComponent* CoreIC = Cast<UCoreInputComponent>(InputComponent);
		if (CoreIC && InputConfigDataAsset)
		{
			CoreIC->BindAbilityActions(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);
		}

		// Lock the latch so we dont bind twice
		bIsInputBound = true;
	}
}

void ACorePlayerCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		// Automatically activates any ability holding this exact tag
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
	}
}

void ACorePlayerCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		// Loops through active specs to tell them the button was released (for abilities that wait for release)
		for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			{
				ASC->AbilitySpecInputReleased(Spec);
			}
		}
	}
}

void ACorePlayerCharacter::UnPossessed()
{
	Super::UnPossessed();

	// Protecting from edge cases where PlayerState possesses something else or gets destroyed
	AbilitySystemComponent = nullptr;
}

UAbilitySystemComponent* ACorePlayerCharacter::GetAbilitySystemComponent() const
{
	ACorePlayerState* PS = GetPlayerState<ACorePlayerState>();
	return PS ? PS->GetAbilitySystemComponent() : nullptr;
}

// Called every frame
void ACorePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACorePlayerCharacter::Move(const FInputActionValue& Value)
{
	// Extract the 2D Axis (WASD or Left Stick)
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Find out which way is "forward" based on where the camera is looking
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get the forward and right vectors based on that yaw
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Feed the input into the Character Movement Component
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACorePlayerCharacter::Look(const FInputActionValue& Value)
{
	// Extract the 2D Axis (Mouse Delta or Right Stick)
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Add yaw and pitch to the controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACorePlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		// Apply the state tag to the character
		// LOOSE GAMEPLAY TAGS DONT REPLICATE, BUT OnStartCrouch and OnEndCrouch RUN ON CLIENT AND SERVER!
		ASC->AddLooseGameplayTag(GASCoreTags::State_Movement_Crouched);
	}
}

void ACorePlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		// Remove the state tag
		ASC->RemoveLooseGameplayTag(GASCoreTags::State_Movement_Crouched);
	}
}

