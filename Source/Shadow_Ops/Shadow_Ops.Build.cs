using UnrealBuildTool;

public class Shadow_Ops : ModuleRules
{
	public Shadow_Ops(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			// Add private modules here if needed
		});

		// Include all public headers inside Source/Shadow_Ops/
		PublicIncludePaths.AddRange(new string[]
		{
			"Shadow_Ops",
			"Shadow_Ops/Enemy"
		});
	}
}