// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "CoreInputConfig.h"
#include "CoreInputComponent.generated.h"

/**
 * UCoreInputComponent
 * * Custom Enhanced Input Component. Acts as the bridge between hardware inputs (UInputAction)
 * and the Gameplay Ability System, using Gameplay Tags as the routing payload.
 */
UCLASS()
class PLAYERCORE_API UCoreInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	/**
	 * Loops through a CoreInputConfig Data Asset and binds all valid Input Actions to the provided functions.
	 * It automatically passes the bound FGameplayTag as a payload to the Pressed/Released functions.
	 * * @tparam UserClass		The class of the object receiving the input (e.g., ACorePlayerCharacter).
	 * @tparam PressedFuncType	The signature of the function to call when input is pressed.
	 * @tparam ReleasedFuncType	The signature of the function to call when input is released.
	 * * @param InputConfig		The Data Asset containing the InputAction-to-GameplayTag mappings.
	 * @param Object			The instance of the UserClass to call the functions on.
	 * @param PressedFunc		The function to fire on ETriggerEvent::Started.
	 * @param ReleasedFunc		The function to fire on ETriggerEvent::Completed.
	 */
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UCoreInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);
};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UCoreInputComponent::BindAbilityActions(const UCoreInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	if (!InputConfig) return;

	for (const FCoreInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
		}
	}
}