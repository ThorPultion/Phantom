#include "GASCoreTags.h"

namespace GASCoreTags
{
	// --- Events ---
	UE_DEFINE_GAMEPLAY_TAG(Event_Death, "Event.Death");
	UE_DEFINE_GAMEPLAY_TAG(Event_Movement_Knockback, "Event.Movement.Knockback");

	// --- Movement States ---
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Crouched, "State.Movement.Crouched");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Airborne, "State.Movement.Airborne");

	// --- Inputs ---
	UE_DEFINE_GAMEPLAY_TAG(Input_Interact, "Input.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Input_SwapAmmo_SetIndex, "Input.SwapAmmo.SetIndex");
	UE_DEFINE_GAMEPLAY_TAG(Input_SwapAmmo_Cycle, "Input.SwapAmmo.Cycle");

	// --- States ---
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_Weapon_Priming, "State.Weapon.Priming");
	UE_DEFINE_GAMEPLAY_TAG(State_Element_Burning, "State.Element.Burning");
	UE_DEFINE_GAMEPLAY_TAG(State_LightSource_Lit, "State.LightSource.Lit");
	UE_DEFINE_GAMEPLAY_TAG(State_AI_Combat, "State.AI.Combat");

	// --- Data ---
	UE_DEFINE_GAMEPLAY_TAG(Data_Magnitude_Force, "Data.Magnitude.Force");
	UE_DEFINE_GAMEPLAY_TAG(Data_Attribute_MaxHealth, "Data.Attribute.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Data_Attribute_MaxEnergy, "Data.Attribute.MaxEnergy");
	UE_DEFINE_GAMEPLAY_TAG(Data_Attribute_MaxDetection, "Data.Attribute.MaxDetection");

	// --- Effects ---
	UE_DEFINE_GAMEPLAY_TAG(Effect_Knockback, "Effect.Knockback");

	// --- GameplayCues ---
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Death_Ragdoll, "GameplayCue.Death.Ragdoll");
	
}