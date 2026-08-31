// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomGameInstance.h"

void UPhantomGameInstance::Init()
{
	if ( GEngine )
	{
		GEngine->OnNetworkFailure().AddUObject(this, &ThisClass::OnConnectionFailure);
	}
}

void UPhantomGameInstance::OnConnectionFailure(UWorld* World,
		UNetDriver* NetDriver,
		ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	if (GEngine)
	{
		switch (FailureType)
		{
		case ENetworkFailure::Type::ConnectionTimeout:
			GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red,
				TEXT("Connection timeout"));
			break;
		default:
			GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red,
				TEXT("Other network error"));
			break;
		}
		
		GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red,
			FString::Printf(TEXT("Connection Failure: %s"), *ErrorString ));
	}
}