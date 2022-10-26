// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FImGuiWrapperEditorCommands : public TCommands<FImGuiWrapperEditorCommands>
{
public:

	FImGuiWrapperEditorCommands()
		: TCommands<FImGuiWrapperEditorCommands>(TEXT("ImGuiWrapperEditorCommands"), NSLOCTEXT("Contexts", "ImGuiWrapperEditor", "ImGuiWrapperEditor Plugin"), NAME_None, FImGuiWrapperEditorCommands::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};