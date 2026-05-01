using UnrealBuildTool;
using System.Collections.Generic;

public class SkyForgeThunderDominionEditorTarget : TargetRules
{
    public SkyForgeThunderDominionEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        CppStandard = CppStandardVersion.Cpp17;
        bOverrideBuildEnvironment = true;
        GlobalDefinitions.Add("__has_feature(x)=0");
        ExtraModuleNames.Add("SkyForgeThunderDominion");
    }
}
