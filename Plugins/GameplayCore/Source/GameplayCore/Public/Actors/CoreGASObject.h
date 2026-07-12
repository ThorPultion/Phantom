// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "CoreGASObject.generated.h"

class UAbilitySystemComponent;
class UCoreAttributeSet;
class UGASInitData;

UCLASS()
class GAMEPLAYCORE_API ACoreGASObject : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoreGASObject();

	// IAbilitySystemInterface implementation
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	const UCoreAttributeSet* AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TObjectPtr<UGASInitData> GASInitData;
};
