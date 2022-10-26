// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UnrealImGui : ModuleRules
{
#if WITH_FORWARDED_MODULE_RULES_CTOR
	public UnrealImGui(ReadOnlyTargetRules Target) : base(Target)
#else
	public UnrealImGui(TargetInfo Target)
#endif
	{
#if WITH_FORWARDED_MODULE_RULES_CTOR
		bool bBuildEditor = Target.bBuildEditor;
#else
		bool bBuildEditor = (Target.Type == TargetRules.TargetType.Editor);
#endif

		// Developer modules are automatically loaded only in editor builds but can be stripped out from other builds.
		// Enable runtime loader, if you want this module to be automatically loaded in runtime builds (monolithic).
		bool bEnableRuntimeLoader = true;
		
#if UE_4_21_OR_LATER
		PrivatePCHHeaderFile = "Private/ImGuiPrivatePCH.h";
#endif

		PublicIncludePaths.AddRange(
			new string[] {
				// Path.Combine(ModuleDirectory, "../ImGuiLibrary/Public")
				// Path.Combine(ModuleDirectory, "Library")
				Path.Combine(ModuleDirectory, "Public/ImGui")
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects",
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"InputCore",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);

		if (bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"EditorStyle",
					"Settings",
					"UnrealEd",
				}
				);
		}


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);


#if !UE_4_19_OR_LATER
		List<string> PrivateDefinitions = Definitions;
#endif

		PrivateDefinitions.Add(string.Format("RUNTIME_LOADER_ENABLED={0}", bEnableRuntimeLoader ? 1 : 0));

	}
}
