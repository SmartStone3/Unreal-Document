#pragma once

#include "MassEntityTraitBase.h"
#include "MassPracticeTrait.generated.h"

UCLASS(meta = (DisplayName = "MassPractice Trait"))
class MASSPRACTICE_API UMassPracticeTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};

