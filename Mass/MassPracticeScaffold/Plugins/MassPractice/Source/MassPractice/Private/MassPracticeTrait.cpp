#include "MassPracticeTrait.h"
#include "MassCommonFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassPracticeFragments.h"

void UMassPracticeTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.RequireFragment<FTransformFragment>();
	BuildContext.AddFragment<FMassPracticeMoveSpeedFragment>();
	BuildContext.AddTag<FMassPracticeAgentTag>();
}

