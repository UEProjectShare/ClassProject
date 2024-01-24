using UnrealBuildTool;
 
public class SecondModule : ModuleRules
{
	public SecondModule(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine"});
 
		PublicIncludePaths.AddRange(new string[] {"SecondModule/Public"});
		PrivateIncludePaths.AddRange(new string[] {"SecondModule/Private"});
	}
}