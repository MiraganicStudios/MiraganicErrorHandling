using System.IO;
using UnrealBuildTool;

public class MiraganicErrorHandlingEditor : ModuleRules
{
	public MiraganicErrorHandlingEditor(ReadOnlyTargetRules Target) : base(Target)
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
				"MiraganicErrorHandling",
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