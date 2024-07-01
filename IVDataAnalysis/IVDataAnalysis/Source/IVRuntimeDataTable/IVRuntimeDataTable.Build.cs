using System.IO;
using UnrealBuildTool;

public class IVRuntimeDataTable : ModuleRules
{
    public IVRuntimeDataTable(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.Add(ModuleDirectory);
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "xlnt"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "utfcpp"));
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
                "Projects",
            }
        );
    }
}