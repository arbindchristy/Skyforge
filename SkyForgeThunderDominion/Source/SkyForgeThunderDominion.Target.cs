using UnrealBuildTool;
using System.Collections.Generic;

public class SkyForgeThunderDominionTarget : TargetRules
{
    public SkyForgeThunderDominionTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        CppStandard = CppStandardVersion.Cpp17;
        bOverrideBuildEnvironment = true;
        GlobalDefinitions.Add("__has_feature(x)=0");
        ExtraModuleNames.Add("SkyForgeThunderDominion");
    }
}
