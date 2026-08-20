// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BlinkArrow.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GASCoreTags.h"
#include "Abilities/GameplayAbilityTypes.h"

void ABlinkArrow::OnProjectileHit(UPrimitiveComponent* HitComponent,
                                  AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnProjectileHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	// We have authority only projectiles thus far, but 
	// clients could still have an arrow hit something thats on a different location on server
	if (!HasAuthority()) return;

	APawn* FiringPawn = GetInstigator();
	if (!FiringPawn) return;
	
	UAbilitySystemComponent* FiringASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(FiringPawn);
	if (!FiringASC) return;
	
	FGameplayEventData Payload;
	Payload.Instigator = FiringPawn;
	Payload.Target = this;

	// GAS uses TSharedPtr internally for TargetData, so we allocate with new here.
	// The handle takes ownership and will safely clean it up later
	FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);
	Payload.TargetData.Add(HitData);
	
	FiringASC->HandleGameplayEvent(GASCoreTags::Event_Movement_Blink, &Payload);
}
