// Distributed under the MIT License (MIT) (see accompanying LICENSE file)

#include "UnrealImGuiModule.h"
#include "ImGuiPrivatePCH.h"

#include "ImGuiModuleManager.h"
#include "ImGuiWidgetEd.h"

#include "ImGuiDelegatesContainer.h"
#include "ImGuiTextureHandle.h"
#include "TextureManager.h"
#include "Utilities/WorldContext.h"
#include "Utilities/WorldContextIndex.h"
#include "Widgets/Docking/SDockTab.h"
#include "WrapperEditor/ImGuiWrapperEditorCommands.h"
#include "WrapperEditor/ImGuiWrapperEditorStyle.h"
#include "LevelEditor.h"

#if WITH_EDITOR
#include "ImGuiImplementation.h"
#include "Editor/ImGuiEditor.h"
#endif

#include "imgui_internal.h"


#define IMGUI_REDIRECT_OBSOLETE_DELEGATES 0

class IPuertsModule;
static const FName ImGuiWrapperEditorTabName("ImGuiWrapperEditor");

#define LOCTEXT_NAMESPACE "FImGuiModule"


struct EDelegateCategory
{
	enum
	{
		// Default per-context draw events.
		Default,

		// Multi-context draw event defined in context manager.
		MultiContext
	};
};

static FImGuiModuleManager* ImGuiModuleManager = nullptr;
FImGuiModuleManager* FUnrealImGuiModule::GetManager()
{
	return ImGuiModuleManager;
}

#if WITH_EDITOR
static FImGuiEditor* ImGuiEditor = nullptr;
#endif

#if IMGUI_WITH_OBSOLETE_DELEGATES

#if WITH_EDITOR
FImGuiDelegateHandle FUnrealImGuiModule::AddEditorImGuiDelegate(const FImGuiDelegate& Delegate)
{
#if IMGUI_REDIRECT_OBSOLETE_DELEGATES
	return { FImGuiDelegatesContainer::Get().OnWorldDebug(Utilities::EDITOR_CONTEXT_INDEX).Add(Delegate),
		EDelegateCategory::Default, Utilities::EDITOR_CONTEXT_INDEX };
#else
	checkf(ImGuiModuleManager, TEXT("Null pointer to internal module implementation. Is module available?"));

	return { ImGuiModuleManager->GetContextManager().GetEditorContextProxy().OnDraw().Add(Delegate),
		EDelegateCategory::Default, Utilities::EDITOR_CONTEXT_INDEX };
#endif // IMGUI_REDIRECT_OBSOLETE_DELEGATES
}
FImGuiDelegateHandle FUnrealImGuiModule::AddEditorWindowImGuiDelegate(const FImGuiDelegate& Delegate, int32 index)
{
#if IMGUI_REDIRECT_OBSOLETE_DELEGATES
    return { FImGuiDelegatesContainer::Get().OnWorldDebug(Utilities::EDITOR_WINDOW_CONTEXT_INDEX_OFFSET + index).Add(Delegate),
        EDelegateCategory::Default, Utilities::EDITOR_WINDOW_CONTEXT_INDEX_OFFSET + index };
#else
    checkf(ImGuiModuleManager, TEXT("Null pointer to internal module implementation. Is module available?"));

    return { ImGuiModuleManager->GetContextManager().GetEditorWindowContextProxy(index).OnDraw().Add(Delegate),
        EDelegateCategory::Default, Utilities::EDITOR_WINDOW_CONTEXT_INDEX_OFFSET + index };
#endif // IMGUI_REDIRECT_OBSOLETE_DELEGATES
}
void FUnrealImGuiModule::ClearEditorWindowImGuiDelegate(int32 index)
{
    checkf(ImGuiModuleManager, TEXT("Null pointer to internal module implementation. Is module available?"));
    ImGuiModuleManager->GetContextManager().GetEditorWindowContextProxy(index).OnDraw().Clear();
}
#endif

FImGuiDelegateHandle FUnrealImGuiModule::AddWorldImGuiDelegate(const FImGuiDelegate& Delegate)
{
#if IMGUI_REDIRECT_OBSOLETE_DELEGATES
	const int32 ContextIndex = Utilities::GetWorldContextIndex((UWorld*)GWorld);
	return { FImGuiDelegatesContainer::Get().OnWorldDebug(ContextIndex).Add(Delegate), EDelegateCategory::Default, ContextIndex };
#else
	checkf(ImGuiModuleManager, TEXT("Null pointer to internal module implementation. Is module available?"));

#if WITH_EDITOR
	checkf(GEngine, TEXT("Null GEngine. AddWorldImGuiDelegate should be only called with GEngine initialized."));

	const FWorldContext* WorldContext = Utilities::GetWorldContext(GEngine->GameViewport);
	if (!WorldContext)
	{
		WorldContext = Utilities::GetWorldContextFromNetMode(ENetMode::NM_DedicatedServer);
	}

	checkf(WorldContext, TEXT("Couldn't find current world. AddWorldImGuiDelegate should be only called from a valid world."));

	int32 Index;
	FImGuiContextProxy& Proxy = ImGuiModuleManager->GetContextManager().GetWorldContextProxy(*WorldContext->World(), Index);
#else
	const int32 Index = Utilities::STANDALONE_GAME_CONTEXT_INDEX;
	FImGuiContextProxy& Proxy = ImGuiModuleManager->GetContextManager().GetWorldContextProxy();
#endif

	return{ Proxy.OnDraw().Add(Delegate), EDelegateCategory::Default, Index };
#endif // IMGUI_REDIRECT_OBSOLETE_DELEGATES
}

FImGuiDelegateHandle FUnrealImGuiModule::AddMultiContextImGuiDelegate(const FImGuiDelegate& Delegate)
{
#if IMGUI_REDIRECT_OBSOLETE_DELEGATES
	return { FImGuiDelegatesContainer::Get().OnMultiContextDebug().Add(Delegate), EDelegateCategory::MultiContext };
#else
	checkf(ImGuiModuleManager, TEXT("Null pointer to internal module implementation. Is module available?"));

	return { ImGuiModuleManager->GetContextManager().OnDrawMultiContext().Add(Delegate), EDelegateCategory::MultiContext };
#endif
}

void FUnrealImGuiModule::RemoveImGuiDelegate(const FImGuiDelegateHandle& Handle)
{
#if IMGUI_REDIRECT_OBSOLETE_DELEGATES
	if (Handle.Category == EDelegateCategory::MultiContext)
	{
		FImGuiDelegatesContainer::Get().OnMultiContextDebug().Remove(Handle.Handle);
	}
	else
	{
		FImGuiDelegatesContainer::Get().OnWorldDebug(Handle.Index).Remove(Handle.Handle);
	}
#else
	if (ImGuiModuleManager)
	{
		if (Handle.Category == EDelegateCategory::MultiContext)
		{
			ImGuiModuleManager->GetContextManager().OnDrawMultiContext().Remove(Handle.Handle);
		}
		else if (auto* Proxy = ImGuiModuleManager->GetContextManager().GetContextProxy(Handle.Index))
		{
			Proxy->OnDraw().Remove(Handle.Handle);
		}
	}
#endif
}

#endif // IMGUI_WITH_OBSOLETE_DELEGATES

FImGuiTextureHandle FUnrealImGuiModule::FindTextureHandle(const FName& Name)
{
	const TextureIndex Index = ImGuiModuleManager->GetTextureManager().FindTextureIndex(Name);
	return (Index != INDEX_NONE) ? FImGuiTextureHandle{ Name, ImGuiInterops::ToImTextureID(Index) } : FImGuiTextureHandle{};
}

FImGuiTextureHandle FUnrealImGuiModule::RegisterTexture(const FName& Name, class UTexture2D* Texture, bool bMakeUnique)
{
	const TextureIndex Index = ImGuiModuleManager->GetTextureManager().CreateTextureResources(Name, Texture, bMakeUnique);
	return FImGuiTextureHandle{ Name, ImGuiInterops::ToImTextureID(Index) };
}

void FUnrealImGuiModule::ReleaseTexture(const FImGuiTextureHandle& Handle)
{
	if (Handle.IsValid())
	{
		ImGuiModuleManager->GetTextureManager().ReleaseTextureResources(ImGuiInterops::ToTextureIndex(Handle.GetTextureId()));
	}
}

void FUnrealImGuiModule::StartupModule()
{
	// Create managers that implements module logic.

	checkf(!ImGuiModuleManager, TEXT("Instance of the ImGui Module Manager already exists. Instance should be created only during module startup."));
	ImGuiModuleManager = new FImGuiModuleManager();

#if WITH_EDITOR
	checkf(!ImGuiEditor, TEXT("Instance of the ImGui Editor already exists. Instance should be created only during module startup."));
	ImGuiEditor = new FImGuiEditor();

	FImGuiWrapperEditorStyle::Initialize();
	FImGuiWrapperEditorStyle::ReloadTextures();
	FImGuiWrapperEditorCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FImGuiWrapperEditorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FUnrealImGuiModule::PluginButtonClicked),
		FCanExecuteAction());
	
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Game", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FUnrealImGuiModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName(TEXT("ImGuiWrapperEditor")), FOnSpawnTab::CreateRaw(this, &FUnrealImGuiModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("ImGuiWrapperEditorTitle", "ImGuiEditor"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
#endif
}

void FUnrealImGuiModule::ShutdownModule()
{
	// In editor store data that we want to move to hot-reloaded module.

#if WITH_EDITOR
	static bool bMoveProperties = true;
	static FImGuiModuleProperties PropertiesToMove = ImGuiModuleManager->GetProperties();
#endif

	// Before we shutdown we need to delete managers that will do all the necessary cleanup.

#if WITH_EDITOR
	checkf(ImGuiEditor, TEXT("Null ImGui Editor. ImGui editor instance should be deleted during module shutdown."));
	delete ImGuiEditor;
	ImGuiEditor = nullptr;
#endif

	// checkf(ImGuiModuleManager, TEXT("Null ImGui Module Manager. Module manager instance should be deleted during module shutdown."));
	// delete ImGuiModuleManager;
	// ImGuiModuleManager = nullptr;

#if WITH_EDITOR
	// When shutting down we leave the global ImGui context pointer and handle pointing to resources that are already
	// deleted. This can cause troubles after hot-reload when code in other modules calls ImGui interface functions
	// which are statically bound to the obsolete module. To keep ImGui code functional we can redirect context handle
	// to point to the new module.

	// When shutting down during hot-reloading, we might want to rewire handles used in statically bound functions
	// or move data to a new module.

	FModuleManager::Get().OnModulesChanged().AddLambda([this] (FName Name, EModuleChangeReason Reason)
	{
		if (Reason == EModuleChangeReason::ModuleLoaded && Name == "UnrealImGui")
		{
			FUnrealImGuiModule& LoadedModule = FUnrealImGuiModule::Get();
			if (&LoadedModule != this)
			{
				// Statically bound functions will be still made to the obsolete module so we need to  
				ImGuiImplementation::SetImGuiContextHandle(LoadedModule.GetImGuiContextHandle());

				FImGuiDelegatesContainer::MoveContainer(LoadedModule.GetDelegatesContainer());

				if (bMoveProperties)
				{
					bMoveProperties = false;
					LoadedModule.SetProperties(PropertiesToMove);
				}
			}
		}
	});
#endif // WITH_EDITOR
}

#if WITH_EDITOR
void FUnrealImGuiModule::SetProperties(const FImGuiModuleProperties& Properties)
{
	ImGuiModuleManager->GetProperties() = Properties;
}

ImGuiContext** FUnrealImGuiModule::GetImGuiContextHandle()
{
	return ImGuiImplementation::GetImGuiContextHandle();
}

FImGuiDelegatesContainer& FUnrealImGuiModule::GetDelegatesContainer()
{
	return FImGuiDelegatesContainer::Get();
}
#endif

FImGuiModuleProperties& FUnrealImGuiModule::GetProperties()
{
	return ImGuiModuleManager->GetProperties();
}

const FImGuiModuleProperties& FUnrealImGuiModule::GetProperties() const
{
	return ImGuiModuleManager->GetProperties();
}

bool FUnrealImGuiModule::IsInputMode() const
{
	return ImGuiModuleManager && ImGuiModuleManager->GetProperties().IsInputEnabled();
}

void FUnrealImGuiModule::SetInputMode(bool bEnabled)
{
	if (ImGuiModuleManager)
	{
		ImGuiModuleManager->GetProperties().SetInputEnabled(bEnabled);
	}
}

void FUnrealImGuiModule::ToggleInputMode()
{
	if (ImGuiModuleManager)
	{
		ImGuiModuleManager->GetProperties().ToggleInput();
	}
}

bool FUnrealImGuiModule::IsShowingDemo() const
{
	return ImGuiModuleManager && ImGuiModuleManager->GetProperties().ShowDemo();
}

void FUnrealImGuiModule::SetShowDemo(bool bShow)
{
	if (ImGuiModuleManager)
	{
		ImGuiModuleManager->GetProperties().SetShowDemo(bShow);
	}
}

void FUnrealImGuiModule::ToggleShowDemo()
{
	if (ImGuiModuleManager)
	{
		ImGuiModuleManager->GetProperties().ToggleDemo();
	}
}

TSharedRef<SDockTab> FUnrealImGuiModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// the unique index to create context for different editor windows
	int ContextIndex = 1;

	// OnGUI function, contains imgui draw instructions
	auto OnGUI = FImGuiDelegate::CreateLambda([this]()
	{
		if (ImGuiWidget.IsValid())
		{
			ImGuiWidget.Get()->OnGUITick();
		}
	});

	// bind the OnGUI function to the Editor Window Context
	FUnrealImGuiModule::Get().ClearEditorWindowImGuiDelegate(ContextIndex);
	FUnrealImGuiModule::Get().AddEditorWindowImGuiDelegate(OnGUI, ContextIndex);

	// create the Slate Widget, the canvas for imgui
	
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			+ SVerticalBox::Slot()
			[
				SAssignNew(ImGuiWidget, SImGuiWidgetEd)
				.ContextIndex(ContextIndex)
			]
		]
		.OnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this, &FUnrealImGuiModule::OnClosePluginTab));
}

void FUnrealImGuiModule::OnClosePluginTab(TSharedRef<SDockTab>)
{
	ImGuiWidget.Reset();
}

void FUnrealImGuiModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(ImGuiWrapperEditorTabName);
}

void FUnrealImGuiModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FImGuiWrapperEditorCommands::Get().OpenPluginWindow);
}

void FUnrealImGuiModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FImGuiWrapperEditorCommands::Get().OpenPluginWindow);
}

void FUnrealImGuiModule::NotifyActiveImGuiInputText(ImGuiInputTextCallbackData* Data)
{
	ImGuiWidget->NotifyActiveImGuiInputText(Data);
}

void FUnrealImGuiModule::OnSyncInputTextBoxTextChanged(const FText& ChangedText)
{
	if (CurrentData)
	{
		if (CurrentData->Buf && !CurrentData->BufDirty)
		{
			strcpy(CurrentData->Buf, TCHAR_TO_ANSI(*ChangedText.ToString()));
		}
		else
		{
			CurrentData = nullptr;
		}
	}
}

//----------------------------------------------------------------------------------------------------
// Runtime loader
//----------------------------------------------------------------------------------------------------

#if !WITH_EDITOR && RUNTIME_LOADER_ENABLED

class FImGuiModuleLoader
{
	FImGuiModuleLoader()
	{
		if (!Load())
		{
			FModuleManager::Get().OnModulesChanged().AddRaw(this, &FImGuiModuleLoader::LoadAndRelease);
		}
	}

	// For different engine versions.
	static FORCEINLINE bool IsValid(const TSharedPtr<IModuleInterface>& Ptr) { return Ptr.IsValid(); }
	static FORCEINLINE bool IsValid(const IModuleInterface* const Ptr) { return Ptr != nullptr; }

	bool Load()
	{
		return IsValid(FModuleManager::Get().LoadModule(ModuleName));
	}

	void LoadAndRelease(FName Name, EModuleChangeReason Reason)
	{
		// Avoid handling own load event.
		if (Name != ModuleName)
		{
			// Try loading until success and then release.
			if (Load())
			{
				FModuleManager::Get().OnModulesChanged().RemoveAll(this);
			}
		}
	}

	static FName ModuleName;

	static FImGuiModuleLoader Instance;
};

FName FImGuiModuleLoader::ModuleName = "UnrealImGui";

// In monolithic builds this will start loading process.
FImGuiModuleLoader FImGuiModuleLoader::Instance;

#endif // !WITH_EDITOR && RUNTIME_LOADER_ENABLED


//----------------------------------------------------------------------------------------------------
// Partial implementations of other classes that needs access to ImGuiModuleManager
//----------------------------------------------------------------------------------------------------

bool FImGuiTextureHandle::HasValidEntry() const
{
	const TextureIndex Index = ImGuiInterops::ToTextureIndex(TextureId);
	return Index != INDEX_NONE && ImGuiModuleManager && ImGuiModuleManager->GetTextureManager().GetTextureName(Index) == Name;
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealImGuiModule, UnrealImGui)
