#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "MassPracticeProcessor.generated.h"

UCLASS()
class MASSPRACTICE_API UMassPracticeProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UMassPracticeProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};

