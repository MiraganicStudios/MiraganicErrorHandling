using UnrealBuildTool;

public class ErrorCodesEditor : ModuleRules
{
	public ErrorCodesEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "GraphEditor",
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
				"ApplicationCore"
			}
		);
	}
}