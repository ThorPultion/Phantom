// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_FocalPoint.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "CoreAIController.h"

void UEnvQueryContext_FocalPoint::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* QuerierObject = QueryInstance.Owner.Get();
	if (!IsValid(QuerierObject)) return;
	
	const ACoreAIController* AIController = Cast<ACoreAIController>(QuerierObject);
	if (!AIController) return;

	// Pulling last known location to use for focal point and handing it to EQS
	UEnvQueryItemType_Point::SetContextHelper(ContextData, AIController->TargetLastKnownLocation);
}