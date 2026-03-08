#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MassEntityConfigAsset.h"
#include "MassPracticeSpawnerActor.generated.h"

UCLASS()
class MASSPRACTICE_API AMassPracticeSpawnerActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "MassPractice")
	TObjectPtr<UMassEntityConfigAsset> EntityConfig = nullptr;

	UPROPERTY(EditAnywhere, Category = "MassPractice", meta = (ClampMin = "1"))
	int32 SpawnCount = 100;

protected:
	virtual void BeginPlay() override;
};

