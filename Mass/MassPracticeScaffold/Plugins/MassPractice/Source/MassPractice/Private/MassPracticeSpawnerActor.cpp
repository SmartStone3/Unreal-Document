#include "MassPracticeSpawnerActor.h"
#include "MassSpawnerSubsystem.h"

void AMassPracticeSpawnerActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World || !EntityConfig)
	{
		return;
	}

	UMassSpawnerSubsystem* SpawnerSubsystem = World->GetSubsystem<UMassSpawnerSubsystem>();
	if (!SpawnerSubsystem)
	{
		return;
	}

	const FMassEntityTemplate& Template = EntityConfig->GetOrCreateEntityTemplate(*World);

	TArray<FMassEntityHandle> SpawnedEntities;
	SpawnerSubsystem->SpawnEntities(Template, (uint32)SpawnCount, SpawnedEntities);
}

