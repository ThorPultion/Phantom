#pragma once

#include "NativeGameplayTags.h"

namespace GASCoreTags
{
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_State_Death);

	// --- Movement States ---
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Crouched);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Airborne);

	// --- Input tags ---
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Interact);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_SwapAmmo_SetIndex);
	GASCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_SwapAmmo_Cycle);
	
}