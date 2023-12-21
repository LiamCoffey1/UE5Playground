// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PlaygroundEditorTarget : TargetRules
{
	public PlaygroundEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        BuildEnvironment = TargetBuildEnvironment.Shared;
        ExtraModuleNames.Add("Playground");
	}
}
