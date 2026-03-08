#pragma once

#include "MassExecutionContext.h"
#include "MassSignalSubsystem.h"

namespace UE::MassPractice
{
	static const FName StartMovingSignal(TEXT("MassPractice.StartMoving"));

	inline void RaiseDeferredStartSignal(FMassExecutionContext& Context, const FMassEntityHandle Entity)
	{
		UMassSignalSubsystem& SignalSubsystem = Context.GetMutableSubsystemChecked<UMassSignalSubsystem>();
		SignalSubsystem.SignalEntityDeferred(Context, StartMovingSignal, Entity);
	}
}

