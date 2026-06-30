#include "GASCoreTags.h"

namespace GASCoreTags
{
	// --- Events ---
	UE_DEFINE_GAMEPLAY_TAG(Event_Death, "Event.Death");

	// --- Movement States ---
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Crouched, "State.Movement.Crouched");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Airborne, "State.Movement.Airborne");

	// --- Inputs ---
	UE_DEFINE_GAMEPLAY_TAG(Input_Interact, "Input.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Input_SwapAmmo_SetIndex, "Input.SwapAmmo.SetIndex");
	UE_DEFINE_GAMEPLAY_TAG(Input_SwapAmmo_Cycle, "Input.SwapAmmo.Cycle");

	// --- States ---
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");
}