// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ItemDefinition.h"

APickupBase::APickupBase()
{
    // No need for tick
    PrimaryActorTick.bCanEverTick = false;

    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
    SetRootComponent(RootComp);

    PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
    PickupMesh->SetupAttachment(RootComp);

    // Blocking only visibility and static objects
    // For interaction linetrace by player
    PickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    PickupMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}

void APickupBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // Blueprint variables are loaded, we can safely check the Data Asset
    if (ItemData && ItemData->PickupMesh)
    {
        // Assign the visual mesh
        PickupMesh->SetStaticMesh(ItemData->PickupMesh);
    }
    else
    {
        // Clear the mesh if Data Asset is removed in editor
        PickupMesh->SetStaticMesh(nullptr);
    }
}

void APickupBase::Interact_Implementation(AActor* Interactor)
{
    if (Interactor && ItemData)
    {
        // 1. Debug message to prove it works
        FString Message = FString::Printf(TEXT("%s just interacted with %s!"), *Interactor->GetName(), *ItemData->ItemName.ToString());
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);

        // 2. TODO: Call the Interactor's Inventory/Equipment Component and pass 'ItemData' to it!

        // 3. Vanish from the world
    }
}

FText APickupBase::GetInteractText_Implementation()
{
    // If the designer remembered to slot in a Data Asset...
    if (ItemData)
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("Verb"), ItemData->GetVerbText());
        Args.Add(TEXT("Item"), ItemData->ItemName);

        // Combines them into "Pick Up Iron Sword"
        return FText::Format(FText::FromString("{Verb} {Item}"), Args);
    }

    // Loud fallback warning if they forgot the Data Asset
    return FText::FromString("ERROR: MISSING ITEM DATA");
}