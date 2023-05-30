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
				"Core",
				"MiraganicErrorHandling",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"ApplicationCore",
				"BlueprintGraph",
			}
		);

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd",
					"GraphEditor",
					"PropertyEditor",
					"EditorStyle",
					"EditorWidgets",
					"KismetCompiler",
				}
			);
		}
	}
}