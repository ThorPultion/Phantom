// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PhantomGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PHANTOM_API UPhantomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
	virtual void OnConnectionFailure(UWorld* World,
		UNetDriver* NetDriver,
		ENetworkFailure::Type FailureType, const FString& ErrorString);
};
