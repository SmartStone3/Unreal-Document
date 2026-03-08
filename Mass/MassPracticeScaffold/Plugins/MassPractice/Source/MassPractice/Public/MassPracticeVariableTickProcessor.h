#pragma once

#include "MassEntityQuery.h"
#include "MassProcessor.h"
#include "MassPracticeVariableTickProcessor.generated.h"

UCLASS()
class MASSPRACTICE_API UMassPracticeVariableTickProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UMassPracticeVariableTickProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};

