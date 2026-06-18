// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoreCharacterBase.h"
#include "CoreAICharacter.generated.h"

UCLASS()
class AICORE_API ACoreAICharacter : public ACoreCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACoreAICharacter();

	// Server side possession initialization for AI
	virtual void PossessedBy(AController* NewController) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitAbilitySystem();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
