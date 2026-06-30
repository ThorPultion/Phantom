#pragma once

#include "NativeGameplayTags.h"

namespace GASCoreTags
{
	// --- Events ---
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Death);

	// --- Movement States ---
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Crouched);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Airborne);

	// --- Inputs ---
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Interact);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_SwapAmmo_SetIndex);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_SwapAmmo_Cycle);
	
	// --- States ---
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);
}