// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ImGuiWrapperEditorCommands.h"

#define LOCTEXT_NAMESPACE "FImGuiWrapperEditorCommands"

void FImGuiWrapperEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "ImGuiEditor", "Bring up ImGuiEditor window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
