using UnrealBuildTool;

public class factory : ModuleRules
{
	public factory(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Headers live in the module root; Core/ and Tests/ include them by name.
		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		// UMG: UUserWidget, UProgressBar, UTextBlock... (the HUD and the emergency minigame are UMG widgets)
		PrivateDependencyModuleNames.AddRange(new string[] { "UMG", "Slate", "SlateCore" });
	}
}
