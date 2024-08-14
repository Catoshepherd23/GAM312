// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAM312 : ModuleRules
{
	public GAM312(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "AIModule" });
	}
}
