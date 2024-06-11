using System.IO;
using UnrealBuildTool;

public class WebpSDK : ModuleRules
{
    public WebpSDK(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        Type = ModuleType.External;
        
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "x64", "libwebp.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "x64", "libwebpdemux.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "x64", "libwebpmux.lib"));
        }
        
        // Add more platform code...
    }
}