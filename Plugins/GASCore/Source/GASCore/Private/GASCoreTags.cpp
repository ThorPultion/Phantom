#include "GASCoreTags.h"

namespace GASCoreTags
{
	UE_DEFINE_GAMEPLAY_TAG(Event_State_Death, "Event.State.Death");

	// --- Movement States ---
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Crouched, "State.Movement.Crouched");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Airborne, "State.Movement.Airborne");

	// --- Input tags ---
	UE_DEFINE_GAMEPLAY_TAG(Input_Interact, "Input.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Input_SwapAmmo_SetIndex, "Input.SwapAmmo.SetIndex");
	UE_DEFINE_GAMEPLAY_TAG(Input_SwapAmmo_Cycle, "Input.SwapAmmo.Cycle");
}