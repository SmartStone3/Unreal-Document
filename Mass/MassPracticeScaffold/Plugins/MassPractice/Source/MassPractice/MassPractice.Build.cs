using UnrealBuildTool;

public class MassPractice : ModuleRules
{
	public MassPractice(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"MassEntity",
				"MassCommon",
				"MassSpawner",
				"MassSimulation",
				"MassMovement",
				"MassSignals",
				"MassLOD"
			}
		);
	}
}

