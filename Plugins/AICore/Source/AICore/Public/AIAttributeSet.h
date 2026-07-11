// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AIAttributeSet.generated.h"

// Boilerplate macro to automatically generate getters and setters for attributes
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * AI specific attributes (Detection, Suspicion, etc.)
 */
UCLASS()
class AICORE_API UAIAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAIAttributeSet();

	// Required for network replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Intercepts attribute changes before they are applied (good for clamping)
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** How aware the AI is of a target (0 = Oblivious, 100 = Full Combat) */
	UPROPERTY(BlueprintReadOnly, Category = "Perception", ReplicatedUsing = OnRep_DetectionLevel)
	FGameplayAttributeData DetectionLevel;
	ATTRIBUTE_ACCESSORS(UAIAttributeSet, DetectionLevel)

	/** The maximum threshold before detection triggers combat (Usually 100) */
	UPROPERTY(BlueprintReadOnly, Category = "Perception", ReplicatedUsing = OnRep_MaxDetection)
	FGameplayAttributeData MaxDetection;
	ATTRIBUTE_ACCESSORS(UAIAttributeSet, MaxDetection)

protected:

	UFUNCTION()
	virtual void OnRep_DetectionLevel(const FGameplayAttributeData& OldDetectionLevel);

	UFUNCTION()
	virtual void OnRep_MaxDetection(const FGameplayAttributeData& OldMaxDetection);
};