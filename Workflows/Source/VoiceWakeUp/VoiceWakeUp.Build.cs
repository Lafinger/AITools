using UnrealBuildTool;

public class VoiceWakeUp : ModuleRules
{
    public VoiceWakeUp(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths required here ...
            }
        );
				
		
        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
                "ThirdParty/VoiceWakeUpSdk/include"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                
                "IVCommon",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                
                "Projects",
                "DeveloperSettings",
                "VoiceWakeUpSdk",
            }
        );
    }
}