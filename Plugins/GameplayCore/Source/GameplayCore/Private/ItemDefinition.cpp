// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDefinition.h"

FText UItemDefinition::GetVerbText() const
{
    // If Custom was chosen from dropdown, return the custom verb input
    if (InteractionType == EInteractionType::Custom)
    {
        // Safety check in case they set it to Custom but left the box blank
        if (CustomInteractionVerb.IsEmpty())
        {
            return FText::FromString("Interact with");
        }
        return CustomInteractionVerb;
    }

    // Centralized translation table for enum/dropdown
    switch (InteractionType)
    {
    case EInteractionType::Pickup:
        return FText::FromString("Pick up");

    case EInteractionType::Open:
        return FText::FromString("Open");

    case EInteractionType::Read:
        return FText::FromString("Read");

    case EInteractionType::Use:
        return FText::FromString("Use");
    default:
        return FText::FromString("Interact with");
    }
}