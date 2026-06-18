// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAIController.h"
#include "Components/StateTreeAIComponent.h"
#include "CoreAIPerceptionComponent.h"

ACoreAIController::ACoreAIController()
{
	CorePerceptionComponent = CreateDefaultSubobject<UCoreAIPerceptionComponent>(TEXT("CorePerceptionComponent"));

	SetPerceptionComponent(*Cast<UAIPerceptionComponent>(CorePerceptionComponent));

	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComponent"));
}

void ACoreAIController::BeginPlay()
{
	Super::BeginPlay();

	// The StateTreeComponent usually starts itself if configured to do so in the Editor,
	// but you can also manually initialize and start it here if needed.
}