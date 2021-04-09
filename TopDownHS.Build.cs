// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TopDownHS : ModuleRules
{
	public TopDownHS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "UMG", "Slate", "SlateCore", "Niagara", "ApexDestruction", "LevelSequence", "MovieScene" });
	}
}
