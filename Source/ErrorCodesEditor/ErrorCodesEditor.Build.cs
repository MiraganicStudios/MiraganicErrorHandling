using System.IO;
using UnrealBuildTool;

public class ErrorCodesEditor : ModuleRules
{
	public ErrorCodesEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Public"),
			}
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"ErrorCodes",
				"GraphEditor",
				"BlueprintGraph",
				"InputCore",
				"PropertyEditor",
				"EditorStyle",
				"EditorWidgets",
				"ApplicationCore",
				"KismetCompiler"
			}
		);
	}
}