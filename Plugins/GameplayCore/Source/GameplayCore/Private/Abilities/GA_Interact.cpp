// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_Interact.h"
#include "CoreCharacterBase.h"
#include "Interactable.h"


void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Casting to base character due to Plugin include limitations
    if (ACoreCharacterBase* BaseCharacter = Cast<ACoreCharacterBase>(ActorInfo->AvatarActor.Get()))
    {
        // Asking base character to get focused interactable.
        // PlayerCharacter from PlayerCore implements the function for real
        if (AActor* Target = BaseCharacter->GetFocusedInteractable())
        {
            if (Target->Implements<UInteractable>())
            {
                IInteractable::Execute_Interact(Target, BaseCharacter);
            }
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}