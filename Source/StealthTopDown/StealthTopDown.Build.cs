// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StealthTopDown : ModuleRules
{
	public StealthTopDown(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule" , "GameplayTasks" });
    }
}
