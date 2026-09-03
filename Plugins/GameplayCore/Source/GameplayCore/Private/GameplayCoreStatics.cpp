// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayCoreStatics.h"
#include "GeneralProjectSettings.h"

FString UGameplayCoreStatics::GetProjectVersion()
{
	FString Version = TEXT("0.0.0");
	
	if (const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>())
	{
		Version = ProjectSettings->ProjectVersion;
	}
	
	return Version;
}
