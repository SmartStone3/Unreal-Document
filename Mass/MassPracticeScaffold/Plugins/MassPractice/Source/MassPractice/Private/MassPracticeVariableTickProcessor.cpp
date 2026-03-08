#include "MassPracticeVariableTickProcessor.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassPracticeFragments.h"
#include "MassSimulationLOD.h"

UMassPracticeVariableTickProcessor::UMassPracticeVariableTickProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::AllNetModes;
	bAutoRegisterWithProcessingPhases = true;
}

void UMassPracticeVariableTickProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassPracticeMoveSpeedFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirement<FMassPracticeAgentTag>(EMassFragmentPresence::All);

	EntityQuery.AddRequirement<FMassSimulationVariableTickFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.AddChunkRequirement<FMassSimulationVariableTickChunkFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.SetChunkFilter(&FMassSimulationVariableTickChunkFragment::ShouldTickChunkThisFrame);
}

void UMassPracticeVariableTickProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [](FMassExecutionContext& Context)
	{
		const TArrayView<FTransformFragment> Transforms = Context.GetMutableFragmentView<FTransformFragment>();
		const TConstArrayView<FMassPracticeMoveSpeedFragment> Speeds = Context.GetFragmentView<FMassPracticeMoveSpeedFragment>();
		const TConstArrayView<FMassSimulationVariableTickFragment> SimTickList = Context.GetFragmentView<FMassSimulationVariableTickFragment>();

		const bool bHasVariableTick = (SimTickList.Num() > 0);
		const float WorldDeltaTime = Context.GetDeltaTimeSeconds();

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
		{
			const float DeltaTime = bHasVariableTick ? SimTickList[EntityIndex].DeltaTime : WorldDeltaTime;
			FTransform& Transform = Transforms[EntityIndex].GetMutableTransform();
			const FVector Forward = Transform.GetRotation().GetForwardVector();
			Transform.AddToTranslation(Forward * Speeds[EntityIndex].Speed * DeltaTime);
		}
	});
}

