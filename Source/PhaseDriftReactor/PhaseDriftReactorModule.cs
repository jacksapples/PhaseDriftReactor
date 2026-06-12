// Copyright JacksApples 2026. All rights reserved.

using UnrealBuildTool;
using System.IO;

public class PhaseDriftReactorModule : ModuleRules
{
    public PhaseDriftReactorModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] {
            "PhaseDriftReactor/Public"
        });

        PrivateIncludePaths.AddRange(new string[] {
            "PhaseDriftReactor/Private"
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "FactoryGame",
            "SML"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "FICSITNetworks"
        });

        DynamicallyLoadedModuleNames.AddRange(new string[] {
            "PhaseDriftReactor"
        });
    }
}
