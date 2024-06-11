using System;
using System.IO;
using System.Collections.Generic;
using UnrealBuildTool;

public class VoiceWakeUpSdk : ModuleRules
{
    private bool isArmArch()
    {
        return Target.Architecture.ToString().ToLower().Contains("arm") || Target.Architecture.ToString().ToLower().Contains("aarch");
    }

    private string GetPlatformLibsSubDirectory()
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            return Path.Combine("libs", "Win");
        }

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            return Path.Combine("libs", "Android");
        }

        if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            return Path.Combine("libs", "iOS");
        }

        string ArchSubDir = isArmArch() ? "Arm64" : "x64";

        if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            return Path.Combine("libs", "Mac", ArchSubDir);
        }

        if (Target.Platform.ToString().ToLower().Contains("linux"))
        {
            return Path.Combine("libs", "Linux", ArchSubDir);
        }

        return "UNDEFINED_DIRECTORY";
    }

    private string GetPlatformLibsAbsoluteDirectory()
    {
        return Path.Combine(ModuleDirectory, GetPlatformLibsSubDirectory());
    }

    private string GetRuntimesSubDirectory()
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            return GetPlatformLibsSubDirectory();
        }

        return GetPlatformLibsSubDirectory();
    }

    private string GetRuntimesAbsoluteDirectory()
    {
        return Path.Combine(ModuleDirectory, GetRuntimesSubDirectory());
    }

    private List<string> GetStaticLibraries()
    {
        List<string> Output = new List<string>();

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            Output.AddRange(new[]
            {
                "msc_x64.lib"
            });
        }
        // else if (Target.Platform == UnrealTargetPlatform.Android)
        // {
        // }
        // else if (Target.Platform.ToString().ToLower().Contains("linux"))
        // {
        // }
        // else if (Target.Platform == UnrealTargetPlatform.IOS)
        // {
        // }
        // else if (Target.Platform == UnrealTargetPlatform.Mac)
        // {
        // }

        return Output;
    }

    private List<string> GetDynamicLibraries()
    {
        List<string> Output = new List<string>();

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            Output.AddRange(new[]
            {
                "msc_x64.dll"
            });
        }
        else if (Target.Platform.ToString().ToLower().Contains("linux") || Target.Platform == UnrealTargetPlatform.Mac)
        {
            // // Using the same list as for static libraries
            // Output = GetStaticLibraries();
        }
        else if (Target.Platform == UnrealTargetPlatform.Android || Target.Platform == UnrealTargetPlatform.IOS)
        {
            // Empty
        }

        return Output;
    }

    private bool IsRuntimePlatform()
    {
        return Target.Platform == UnrealTargetPlatform.Win64 ||
            Target.Platform == UnrealTargetPlatform.Mac ||
            Target.Platform.ToString().ToLower().Contains("linux");
    }

    private void InitializeRuntimeDefinitions()
    {
        if (!IsRuntimePlatform() || GetDynamicLibraries().Count <= 0)
        {
            return;
        }

        PublicDefinitions.Add(string.Format("VOICEWAKEUP_WHITELISTED_BINARIES=\"{0}\"", string.Join(";", GetDynamicLibraries())));

        if (Target.Type != TargetType.Editor)
        {
            return;
        }

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDefinitions.Add(string.Format("VOICEWAKEUP_THIRDPARTY_BINARY_SUBDIR=\"{0}\"", GetRuntimesSubDirectory().Replace(@"\", @"\\")));
            PublicDefinitions.Add(string.Format("VOICEWAKEUP_THIRDPARTY_RESOURCE_FILE=\"{0}\"", Path.Combine("res", "wakeupresource.jet").Replace(@"\", @"\\")));
        }
        // else if (Target.Platform == UnrealTargetPlatform.Mac || Target.Platform.ToString().ToLower().Contains("linux"))
        // {
        //     PublicDefinitions.Add(string.Format("VOICEWAKEUP_THIRDPARTY_BINARY_SUBDIR=\"{0}\"", GetRuntimesSubDirectory().Replace(@"\", @"/")));
        // }
        
        
    }

    public VoiceWakeUpSdk(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        // CppStandard = CppStandardVersion.Cpp17;
        Type = ModuleType.External;
        bEnableExceptions = true;

        PublicIncludePaths.AddRange(new[]
        {
            Path.Combine(ModuleDirectory, "include")
        });

        InitializeRuntimeDefinitions();

        foreach (string StaticLib in GetStaticLibraries())
        {
            PublicAdditionalLibraries.Add(Path.Combine(GetPlatformLibsAbsoluteDirectory(), StaticLib));
        }

        foreach (string DynamicLib in GetDynamicLibraries())
        {
            PublicDelayLoadDLLs.Add(DynamicLib);
            RuntimeDependencies.Add(Path.Combine(@"$(BinaryOutputDir)", DynamicLib), Path.Combine(GetRuntimesAbsoluteDirectory(), DynamicLib));
        }

        // if (Target.Platform == UnrealTargetPlatform.Mac)
        // {
        //     // Experimental UPL usage for MacOS to add the required PList data
        //     AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(ModuleDirectory, "AzSpeech_UPL_MacOS.xml"));
        // }
        // else if (Target.Platform == UnrealTargetPlatform.IOS)
        // {
        //     AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(ModuleDirectory, "AzSpeech_UPL_IOS.xml"));
        // }
        // else if (Target.Platform == UnrealTargetPlatform.Android)
        // {
        //     AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "AzSpeech_UPL_Android.xml"));
        // }
        
        
        string ResFiles = Path.Combine(ModuleDirectory, "res");
        RuntimeDependencies.Add(String.Format("{0}/*", ResFiles));
    }
}