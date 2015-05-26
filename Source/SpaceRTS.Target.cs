// Pyrite Softworks 2015

using UnrealBuildTool;
using System.Collections.Generic;

public class SpaceRTSTarget : TargetRules
{
    public SpaceRTSTarget(TargetInfo Target)
	{
		Type = TargetType.Game;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "SpaceRTS" } );
	}
}
