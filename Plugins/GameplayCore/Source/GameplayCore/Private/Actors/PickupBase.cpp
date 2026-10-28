// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PickupBase.h"
#include "Components/StaticMeshComponent.h"
#include "Data/ItemDefinition.h"
#include "Net/UnrealNetwork.h"

APickupBase::APickupBase()
{
    // No need for tick
    PrimaryActorTick.bCanEverTick = false;

    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
    SetRootComponent(RootComp);

    PickupCollider = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupCollider"));
    PickupCollider->SetupAttachment(RootComp);

    // Blocking only visibility and static objects
    // For interaction linetrace by player
    PickupCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    PickupCollider->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    
    PickupCollider->SetVisibility(false);
    
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(RootComp);
    
    bReplicates = true;
}

void APickupBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // Blueprint variables are loaded, we can safely check definition Data Asset
    OnRep_ItemData();
}

void APickupBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(APickupBase, ItemData);
}

void APickupBase::ChangeItem(UItemDefinition* InItemData)
{
    ItemData = InItemData;
    
    if (HasAuthority())
    {
        OnRep_ItemData();
    }
}

void APickupBase::OnRep_ItemData()
{
    if (ItemData && ItemData->PickupMesh)
    {
        // Assigning the visual mesh and the scaled up collider
        VisualMesh->SetStaticMesh(ItemData->PickupMesh);
        PickupCollider->SetStaticMesh(ItemData->PickupMesh);
    }
    else
    {
        // Clear the mesh if Data Asset is removed in editor
        VisualMesh->SetStaticMesh(nullptr);
        PickupCollider->SetStaticMesh(nullptr);
    }
}

void APickupBase::Interact_Implementation(AActor* Interactor)
{
    if (Interactor && ItemData)
    {
        // If there is something we want to do on every pickup interact that would go here
    }
}

FText APickupBase::GetInteractText_Implementation()
{
    if (ItemData)
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("Verb"), ItemData->GetVerbText());
        Args.Add(TEXT("Item"), ItemData->ItemName);

        // Combines them into "Pick Up Iron Sword"
        return FText::Format(FText::FromString("{Verb} {Item}"), Args);
    }

    return FText::FromString("ERROR: MISSING ITEM DATA");
}