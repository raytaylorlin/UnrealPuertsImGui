// Copyright Epic Games, Inc. All Rights Reserved.

#include "PuertsXImGui.h"

#include "PuertsModule.h"
#include "UnrealImGuiModule.h"

#define LOCTEXT_NAMESPACE "FPuertsXImGuiModule"

void FPuertsXImGuiModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FUnrealImGuiModule& UnrealImGuiModule = FUnrealImGuiModule::Get();
	UnrealImGuiModule.OnImGuiWidgetConstruct.AddRaw(this, &FPuertsXImGuiModule::StartJsEnv);
	UnrealImGuiModule.OnImGuiWidgetUpdate.AddRaw(this, &FPuertsXImGuiModule::OnUpdateImGui);
	UnrealImGuiModule.OnImGuiWidgetDestroy.AddRaw(this, &FPuertsXImGuiModule::StopJsEnv);

	IPuertsModule& PuertsModule = FModuleManager::LoadModuleChecked<IPuertsModule>("Puerts");
	PuertsModule.OnModuleChange.AddLambda([this](FName Path)
	{
		if (!Path.ToString().StartsWith("EditorEntry/"))
		{
			return;
		}
		RestartJsEnv();
	});
}

void FPuertsXImGuiModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FPuertsXImGuiModule::StartJsEnv()
{
	JsEnv = MakeShared<puerts::FJsEnv>();
	TArray<TPair<FString, UObject*>> Arguments;
	PuertsXImGuiBridge = NewObject<UPuertsXImGuiBridge>();
	// 将中介UObject暴露给TS使用
	Arguments.Add(TPair<FString, UObject*>(TEXT("ImGuiBridge"), PuertsXImGuiBridge));
	JsEnv->Start("EditorEntry/Main", Arguments);
}

void FPuertsXImGuiModule::StopJsEnv()
{
	if (PuertsXImGuiBridge)
	{
		PuertsXImGuiBridge->ConditionalBeginDestroy();
		PuertsXImGuiBridge = nullptr;
	}
	if (JsEnv)
	{
		JsEnv.Reset();
	}
}

void FPuertsXImGuiModule::RestartJsEnv()
{
	StopJsEnv();
	StartJsEnv();
}

void FPuertsXImGuiModule::OnUpdateImGui()
{
	if (PuertsXImGuiBridge && PuertsXImGuiBridge->OnGUI.IsBound())
	{
		PuertsXImGuiBridge->OnGUI.Broadcast();
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPuertsXImGuiModule, PuertsXImGui)