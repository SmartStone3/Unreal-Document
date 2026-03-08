#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MassPracticeFragments.generated.h"

USTRUCT()
struct FMassPracticeMoveSpeedFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Speed = 300.f;
};

USTRUCT()
struct FMassPracticeAgentTag : public FMassTag
{
	GENERATED_BODY()
};

