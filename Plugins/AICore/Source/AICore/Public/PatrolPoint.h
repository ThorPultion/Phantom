// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPoint.generated.h"

class UArrowComponent;
class UStaticMeshComponent;

UCLASS()
class AICORE_API APatrolPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolPoint();
	
	/** Returns true if the patrol point is occupied */
	UFUNCTION(BlueprintPure, Category = "Patrol")
	bool IsClaimed() const { return bIsClaimed; }
	
	/** Claim patrol point so other AI cannot use it */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	void Claim(AActor* Claimer);
	
	/** Let other AI claim the patrol point again */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	void Release();

	/** How long the AI should wait before moving on to next point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float WaitTime = 5.0f;

	/** What the AI should do while waiting at the patrol point */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	//FGameplayTag StationActionTag;

protected:
	
	virtual void BeginPlay() override;
	
	/** Points the direction the AI should look if we care about doing that */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UArrowComponent> FacingDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> EditorIndicator;

private:
	/** Tracks if point is claimed */
	UPROPERTY(VisibleAnywhere, Category = "Patrol")
	bool bIsClaimed = false;
    
	/** Tracks who is claiming point (usually AI) */
	UPROPERTY(VisibleAnywhere, Category = "Patrol")
	TObjectPtr<AActor> CurrentClaimer = nullptr;

};
