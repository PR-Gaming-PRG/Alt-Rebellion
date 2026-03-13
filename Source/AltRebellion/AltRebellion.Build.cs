// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AltRebellion : ModuleRules
{
	public AltRebellion(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"AltRebellion",
			"AltRebellion/Variant_Strategy",
			"AltRebellion/Variant_Strategy/UI",
			"AltRebellion/Variant_TwinStick",
			"AltRebellion/Variant_TwinStick/AI",
			"AltRebellion/Variant_TwinStick/Gameplay",
			"AltRebellion/Variant_TwinStick/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
