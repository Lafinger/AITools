using UnrealBuildTool;

public class VoiceRecognition : ModuleRules
{
    public VoiceRecognition(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Common", 
                "AudioCaptureCore", 
                "AudioCapture",
                "AudioMixer",
                "Json", 
                
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
                
                "DeveloperSettings",
            }
        );
    }
}