using UnrealBuildTool;

public class factory : ModuleRules
{
	public factory(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		// UMG: UUserWidget, UProgressBar, UTextBlock... (the HUD and the emergency minigame are UMG widgets)
		PrivateDependencyModuleNames.AddRange(new string[] { "UMG", "Slate", "SlateCore" });
	}
}
