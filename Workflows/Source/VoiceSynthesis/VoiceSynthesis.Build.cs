﻿using UnrealBuildTool;

public class VoiceSynthesis : ModuleRules
{
    public VoiceSynthesis(ReadOnlyTargetRules Target) : base(Target)
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
                "Slate",
                "SlateCore",
                
                "IVXunFeiSpeech",
            }
        );
    }
}