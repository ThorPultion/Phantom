// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "EnvQueryGenerator_PatrolPoints.generated.h"

/**
 * 
 */
UCLASS()
class AICORE_API UEnvQueryGenerator_PatrolPoints : public UEnvQueryGenerator
{
	GENERATED_BODY()
	
public:
	explicit UEnvQueryGenerator_PatrolPoints(const FObjectInitializer& ObjectInitializer);

	// This is the core function where we feed items to the query
	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	// Editor UI Text
	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
};
