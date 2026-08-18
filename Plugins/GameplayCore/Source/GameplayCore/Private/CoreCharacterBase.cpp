// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "CoreAbilitySystemComponent.h"
#include "Components/EquipmentComponent.h"
#include "CoreAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GASCoreTags.h"
#include "Data/GASInitData.h"
#include "Perception/AIPerceptionTypes.h"

// Sets default values
ACoreCharacterBase::ACoreCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentManager"));
	EquipmentComponent->SetIsReplicated(true);
}

UAbilitySystemComponent* ACoreCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACoreCharacterBase::GrantStartingAbilities()
{
    UCoreAbilitySystemComponent* ASC = Cast<UCoreAbilitySystemComponent>(GetAbilitySystemComponent());

    if (!HasAuthority() || !ASC || !GASInitData || !GASInitData->StartingAbilities) return;

	ASC->GrantAbilitySetAsync(GASInitData->StartingAbilities);
}

void ACoreCharacterBase::SetupAttributes()
{
	if (!HasAuthority() || !AbilitySystemComponent || !GASInitData || !GASInitData->InitializationEffect) return;

	// Initializing attributes
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(GASInitData);

	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		GASInitData->InitializationEffect, 1.f, ContextHandle);

	if (SpecHandle.IsValid())
	{
		// Pulling stats from data asset
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(GASCoreTags::Data_Attribute_MaxHealth, GASInitData->MaxHealth);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(GASCoreTags::Data_Attribute_MaxEnergy, GASInitData->MaxEnergy);

		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ACoreCharacterBase::GrantPassiveEffects()
{
	if (!HasAuthority() || !AbilitySystemComponent || !GASInitData || GASInitData->PassiveEffects.IsEmpty()) return;

	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddInstigator(this, this);

	// Applying passive effects and tags
	for (TSubclassOf<UGameplayEffect> EffectClass : GASInitData->PassiveEffects)
	{
		if (EffectClass)
		{
			const UGameplayEffect* BaseEffect = EffectClass->GetDefaultObject<UGameplayEffect>();
			AbilitySystemComponent->ApplyGameplayEffectToSelf(BaseEffect, 1.f, ContextHandle);
		}
	}
}

void ACoreCharacterBase::GrantStartingEquipment()
{
	const UCoreAbilitySystemComponent* ASC = Cast<UCoreAbilitySystemComponent>(GetAbilitySystemComponent());

	if (!HasAuthority() || !ASC || !EquipmentComponent || EquipmentComponent->StartingEquipment.IsEmpty()) return;

	for (UEquipmentDefinition* StartingItem : EquipmentComponent->StartingEquipment)
	{
		if (StartingItem)
		{
			EquipmentComponent->AddItemToLoadout(StartingItem);
		}
	}
}

// Called when the game starts or when spawned
void ACoreCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

void ACoreCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Running on the Server when the controller takes possession
	InitAbilitySystem();

	// ASC is initiated, so give starting abilities
	GrantStartingAbilities();

	SetupAttributes();

	GrantPassiveEffects();

	// Equipment grant abilities as well
	GrantStartingEquipment();
}

// Called every frame
void ACoreCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACoreCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACoreCharacterBase::InitAbilitySystem()
{
	// IMPORTANT:
	// This Super should be called AFTER initializing ASC in inheriting classes!
	if (AbilitySystemComponent && AttributeSet)
	{
		BindAbilitySystemDelegates();

		// IMPORTANT: For external systems that want to know things about ASC on BeginPlay, Construct or such.
		// Fetch ASC, if ASC is valid, AbilitySystemInit ran first, no need to do anything.
		// If fetched ASC is not valid, bind to listen to this delegate.
		// The full process above will negate the race condition.
		OnAbilitySystemInitialized.Broadcast(AbilitySystemComponent);
	}
}

void ACoreCharacterBase::BindAbilitySystemDelegates()
{
	// This Super should be called AFTER initializing ASC!
	if (AbilitySystemComponent && AttributeSet)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetMovementSpeedAttribute()).Remove(MovementSpeedChangedDelegateHandle);

		// Start listening for movement speed changes and store handle for cleanup
		MovementSpeedChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetMovementSpeedAttribute()).AddUObject(this, &ACoreCharacterBase::OnMovementSpeedChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(
			GASCoreTags::State_Weapon_Priming,
			EGameplayTagEventType::NewOrRemoved).Remove(AimingTagDelegateHandle);

		// Start listening for aiming GE and store handle for cleanup
		AimingTagDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(
			GASCoreTags::State_Weapon_Priming,
			EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ACoreCharacterBase::OnAimingTagChanged);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(
			GASCoreTags::State_Dead, 
			EGameplayTagEventType::NewOrRemoved).Remove(DeadTagDelegateHandle);
		
		DeadTagDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(
			GASCoreTags::State_Dead, 
			EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ACoreCharacterBase::OnDeadTagChanged);
	}
}

void ACoreCharacterBase::OnMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	// Apply the new GAS value directly to the engines movement component
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = Data.NewValue;
	}
}

void ACoreCharacterBase::OnAimingTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// If the count is > 0, the GE is active
	bIsAiming = (NewCount > 0);
}

FGenericTeamId ACoreCharacterBase::GetGenericTeamId() const
{
	return TeamID;
}

FGameplayTag ACoreCharacterBase::GetPerceptionTag_Implementation(ETeamAttitude::Type ObserverAttitude, const FAIStimulus& Stimulus)
{
	// If we are dead
	if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(GASCoreTags::State_Dead))
	{
		return Stimulus.WasSuccessfullySensed() ? ReactionData->SensedDeadTag : FGameplayTag::EmptyTag;
	}

	if (ObserverAttitude == ETeamAttitude::Hostile)
	{
		// The AI must sense the player and the player must be visible enough
		if (Stimulus.WasSuccessfullySensed() && 
			Execute_GetVisibilityModifier(this) > Execute_GetInvisibleThreshold(this))
		{
			return ReactionData->SensedAliveTag;
		}
		else
		{
			// Triggers if player walks behind a wall or if they step into the dark
			return ReactionData->LostSightTag;
		}
	}
	
	if (ObserverAttitude == ETeamAttitude::Friendly)
	{
		// If we arent successfully sensed by our teammate, ignore
		if (!Stimulus.WasSuccessfullySensed())
		{
			return FGameplayTag::EmptyTag;
		}

		// Check if we (the observed character) are currently in combat or active state
		if (AbilitySystemComponent && ReactionData)
		{
			// If we are actively fighting or alerted, tell our teammates to assist us
			if (AbilitySystemComponent->HasMatchingGameplayTag(GASCoreTags::State_AI_Combat) || 
				AbilitySystemComponent->HasMatchingGameplayTag(GASCoreTags::State_AI_Searching) ||
				AbilitySystemComponent->HasMatchingGameplayTag(GASCoreTags::State_AI_Suspicious))
			{
				return ReactionData->TeamAssistTag;
			}
		}
	}
	
	return FGameplayTag::EmptyTag;
}

FVector ACoreCharacterBase::GetTargetLocation(AActor* RequestedBy) const
{
	if (GetMesh())
	{
		// AI SetFocus will now aim at the designated socket
		return GetMesh()->GetSocketLocation(SetFocusTargetSocket);
	}
    
	// Fallback
	return Super::GetTargetLocation(RequestedBy);
}

float ACoreCharacterBase::GetVisibilityModifier_Implementation()
{
	return 1.0f;
}

UAISense_Sight::EVisibilityResult ACoreCharacterBase::CanBeSeenFrom(const FCanBeSeenFromContext& Context, FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	// ------Our light/dark based stealth check STARTS-------
	const float CurrentVisibility = IPerceivable::Execute_GetVisibilityModifier(this);
	const float StealthThreshold = IPerceivable::Execute_GetInvisibleThreshold(this);

	if (CurrentVisibility <= StealthThreshold)
	{
		// Player is too dark to be seen
		OutSightStrength = 0.0f;
		return UAISense_Sight::EVisibilityResult::NotVisible; 
	}
	// ------Our light/dark based stealth check ENDS-------

	// IMPLEMENTING A SOFT EQUIVALENT OF AISense_Sight DEFAULT CHECKER
	// We must do this, because implementing this interface intercepts the default check of sight sense
	FHitResult HitResult;
	const FVector TargetLocation = GetActorLocation();
	
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(AILineOfSight), true, Context.IgnoreActor);
	QueryParams.AddIgnoredActor(this);
	
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,
		Context.ObserverLocation,
		TargetLocation,
		ECC_Visibility, QueryParams);
    
	OutNumberOfLoSChecksPerformed++;
	OutNumberOfAsyncLosCheckRequested = 0; // We are doing a synchronous trace

	// If we hit nothing or ourselves, we are physically visible
	if (!bHit || HitResult.GetActor() == this)
	{
		OutSeenLocation = TargetLocation;
		OutSightStrength = 1.0f;
		return UAISense_Sight::EVisibilityResult::Visible;
	}

	// We hit a wall
	OutSightStrength = 0.0f;
	return UAISense_Sight::EVisibilityResult::NotVisible;
}

void ACoreCharacterBase::OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// This runs when the character dies
}