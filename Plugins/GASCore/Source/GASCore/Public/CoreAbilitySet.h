#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "CoreAbilitySet.generated.h"

class UCoreGameplayAbility;

USTRUCT(BlueprintType)
struct FCoreGameplayAbilityBindInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	TSoftClassPtr<UCoreGameplayAbility> AbilityClass;

	/** The Gameplay Tag used by the custom Input Component to activate this ability */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FGameplayTag InputTag;
};

UCLASS(BlueprintType)
class GASCORE_API UCoreAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<FCoreGameplayAbilityBindInfo> Abilities;
};