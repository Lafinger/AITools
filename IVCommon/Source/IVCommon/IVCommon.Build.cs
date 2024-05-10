// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class IVCommon : ModuleRules
{
	public IVCommon(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"DesktopPlatform",
					"MessageLog",
					"UnrealEd",
				}
			);
		}
		
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
				"Slate",
				"SlateCore",
				"Json",
				"RHI",
				"RenderCore",
				"Projects",
				"DeveloperSettings",
				"JsonUtilities",
                "AudioCapture",
                "AudioMixer",
            }
		);

		string ProjectConfigDir = "$(ProjectDir)/Config/ProjectJsonConfig";
		RuntimeDependencies.Add(String.Format("{0}/*", ProjectConfigDir));
		
		string ActorJsonConfigDir = "$(ProjectDir)/Config/ActorJsonConfig";
		RuntimeDependencies.Add(String.Format("{0}/*", ActorJsonConfigDir));
		
		string IVExtrasDir = Path.Combine(PluginDirectory, "Extras");
		foreach (string DataFile in Directory.EnumerateFiles(IVExtrasDir, "*", SearchOption.AllDirectories))
		{
			RuntimeDependencies.Add(DataFile);
		}
	}
}
