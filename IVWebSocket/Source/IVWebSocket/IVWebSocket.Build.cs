// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IVWebSocket : ModuleRules
{
	public IVWebSocket(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"WebSockets",
				"IVCommon",
			}
		);
	}
}
