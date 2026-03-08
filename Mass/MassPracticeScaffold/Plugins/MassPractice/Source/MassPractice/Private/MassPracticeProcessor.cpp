#include "MassPracticeProcessor.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassPracticeFragments.h"

UMassPracticeProcessor::UMassPracticeProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::AllNetModes;
	bAutoRegisterWithProcessingPhases = true;
}

void UMassPracticeProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassPracticeMoveSpeedFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirement<FMassPracticeAgentTag>(EMassFragmentPresence::All);
}

void UMassPracticeProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [](FMassExecutionContext& Context)
	{
		const float DeltaTime = Context.GetDeltaTimeSeconds();
		const TArrayView<FTransformFragment> Transforms = Context.GetMutableFragmentView<FTransformFragment>();
		const TConstArrayView<FMassPracticeMoveSpeedFragment> Speeds = Context.GetFragmentView<FMassPracticeMoveSpeedFragment>();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
		{
			FTransform& Transform = Transforms[EntityIndex].GetMutableTransform();
			const FVector Forward = Transform.GetRotation().GetForwardVector();
			Transform.AddToTranslation(Forward * Speeds[EntityIndex].Speed * DeltaTime);
		}
	});
}

