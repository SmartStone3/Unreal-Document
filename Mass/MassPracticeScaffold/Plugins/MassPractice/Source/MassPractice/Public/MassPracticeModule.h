#pragma once

#include "Modules/ModuleManager.h"

class FMassPracticeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

