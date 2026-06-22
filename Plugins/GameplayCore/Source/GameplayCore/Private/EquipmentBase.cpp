// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentBase.h"

// Sets default values
AEquipmentBase::AEquipmentBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	// Dummy root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonMesh"));
	ThirdPersonMesh->SetupAttachment(RootComponent);

	ThirdPersonMesh->SetOwnerNoSee(true);
	ThirdPersonMesh->SetCastHiddenShadow(true);

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(RootComponent);

	// Could create a high and low res version of each weapon, so that
	// first person is optimized but other players see an optimized version.
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->SetCastShadow(false);
}

// Called when the game starts or when spawned
void AEquipmentBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEquipmentBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

