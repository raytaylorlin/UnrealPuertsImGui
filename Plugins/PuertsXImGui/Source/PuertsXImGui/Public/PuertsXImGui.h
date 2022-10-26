// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "JsEnv.h"
#include "UnrealImGuiModule.h"
#include "PuertsXImGuiBridge.h"

class FPuertsXImGuiModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	TSharedPtr<puerts::FJsEnv> JsEnv;
	UPuertsXImGuiBridge* PuertsXImGuiBridge = nullptr;
	void StartJsEnv();
	void StopJsEnv();
	void RestartJsEnv();
	void OnUpdateImGui();
};
