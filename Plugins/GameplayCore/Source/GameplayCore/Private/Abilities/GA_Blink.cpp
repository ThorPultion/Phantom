// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_Blink.h"

void UGA_Blink::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
    
	if (AvatarActor && TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		const FGameplayAbilityTargetData* TargetData = TriggerEventData->TargetData.Get(0);
        
		// HasEndPoint() returns true if it finds either a HitResult or a LocationInfo
		if (TargetData && TargetData->HasEndPoint())
		{
			// Extracts the ImpactPoint from a hit, or the raw vector from LocationInfo
			FVector BlinkLocation = TargetData->GetEndPointTransform().GetLocation();
			
			
			const FHitResult* HitResult = TargetData->GetHitResult();
			if (HitResult && HitResult->bBlockingHit)
			{
				float CollisionRadius = 0.0f;
				float CollisionHalfHeight = 0.0f;
				AvatarActor->GetSimpleCollisionCylinder(CollisionRadius, CollisionHalfHeight);
				
				const float CylinderHalfHeight = CollisionHalfHeight - CollisionRadius;
              
				// We scale the cylinder part by the verticality of the normal.
				// If Normal.Z is 1 (floor) or -1 (ceiling), it returns the full HalfHeight.
				// If Normal.Z is 0 (wall), it returns just the Radius.
				const float OffsetMagnitude = CollisionRadius + (CylinderHalfHeight * FMath::Abs(HitResult->ImpactNormal.Z));
				
				const FVector NormalOffset = HitResult->ImpactNormal * (OffsetMagnitude + 2.0f);

				BlinkLocation += NormalOffset;
			}
            
			AvatarActor->SetActorLocation(BlinkLocation);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}