#include "ImGuiWidgetEd.h"
#include "ImGuiPrivatePCH.h"

#include "ImGuiContextManager.h"
#include "ImGuiContextProxy.h"
#include "ImGuiInputHandler.h"
#include "ImGuiInputHandlerFactory.h"
#include "ImGuiModuleManager.h"
#include "ImGuiModuleProperties.h"
#include "Input/Reply.h"

#include "ImGuiTextInputMethodContext.h"
#include "imgui_internal.h"

void SImGuiWidgetEd::Construct(const FArguments& InArgs)
{
	// checkf(InArgs._ModuleManager, TEXT("Null Module Manager argument"));
	ModuleManager = FUnrealImGuiModule::GetManager();
	ContextIndex_UnOffset = InArgs._ContextIndex;
	ModuleManager->OnPostImGuiUpdate().AddRaw(this, &SImGuiWidgetEd::OnPostImGuiUpdate);
	ModuleManager->GetProperties().SetInputEnabledInEditor(true);
	ModuleManager->LoadTextures();

	const auto& Settings = ModuleManager->GetSettings();
	CreateInputHandler(Settings.GetImGuiInputHandlerClass());

	ImGuiIO& IO = ImGui::GetIO();
	IO.FontDefault = IO.Fonts->Fonts[IO.Fonts->Fonts.Size - 1];
	
	TextInputMethodContext = FImGuiTextInputMethodContext::Create(SharedThis(this));
	
	FUnrealImGuiModule& UnrealImGuiModule = FUnrealImGuiModule::Get();
	if (UnrealImGuiModule.OnImGuiWidgetConstruct.IsBound())
	{
		UnrealImGuiModule.OnImGuiWidgetConstruct.Broadcast();
	}
}

void SImGuiWidgetEd::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	Super::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	if (TextInputMethodChangeNotifier.IsValid() && TextInputMethodContext.IsValid() && TextInputMethodContext->UpdateCachedGeometry(AllottedGeometry))
	{
		TextInputMethodChangeNotifier->NotifyLayoutChanged(ITextInputMethodChangeNotifier::ELayoutChangeType::Changed);
	}

	UpdateInputState();
}

void SImGuiWidgetEd::OnGUITick()
{
	// 去掉这个注释可以测试C++版的ImGui
	// ImGui::ShowDemoWindow();

	FUnrealImGuiModule& UnrealImGuiModule = FUnrealImGuiModule::Get();
	if (UnrealImGuiModule.OnImGuiWidgetUpdate.IsBound())
	{
		UnrealImGuiModule.OnImGuiWidgetUpdate.Broadcast();
	}
}

SImGuiWidgetEd::~SImGuiWidgetEd()
{
	ModuleManager->OnPostImGuiUpdate().RemoveAll(this);
	ModuleManager->GetProperties().SetInputEnabledInEditor(false);
	FUnrealImGuiModule& UnrealImGuiModule = FUnrealImGuiModule::Get();
	if (UnrealImGuiModule.OnImGuiWidgetDestroy.IsBound())
	{
		UnrealImGuiModule.OnImGuiWidgetDestroy.Broadcast();
	}
	ReleaseInputHandler();
}

void SImGuiWidgetEd::CreateInputHandler(const FStringClassReference& HandlerClassReference)
{
	ReleaseInputHandler();

	if (!InputHandler.IsValid())
	{
		InputHandler = FImGuiInputHandlerFactory::NewHandler(GetInputHandlerOuterPkg(), HandlerClassReference, ModuleManager, ContextIndex_UnOffset);
	}

}

UPackage* SImGuiWidgetEd::GetInputHandlerOuterPkg()
{
    static UPackage* InputHandlerOuterPkg = nullptr;
    if (InputHandlerOuterPkg == nullptr)
	{
		InputHandlerOuterPkg = CreatePackage(nullptr, TEXT("/Temp/InputHandlerOuter"));
		// InputHandlerOuterPkg->AddToRoot();
	}
    return InputHandlerOuterPkg;
}

void SImGuiWidgetEd::ReleaseInputHandler()
{
	if (InputHandler.IsValid())
	{
		FImGuiInputHandlerFactory::ReleaseHandler(InputHandler.Get());
		InputHandler.Reset();
	}
}

void SImGuiWidgetEd::UpdateInputState()
{
	auto& Properties = ModuleManager->GetProperties();

	bInputEnabled = Properties.IsInputEnabledInEditor();
}

FReply SImGuiWidgetEd::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent)
{
	return InputHandler->OnKeyChar(CharacterEvent);
}

FReply SImGuiWidgetEd::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	//UpdateCanvasControlMode(KeyEvent);
	return InputHandler->OnKeyDown(KeyEvent);
}

FReply SImGuiWidgetEd::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	//UpdateCanvasControlMode(KeyEvent);
	return InputHandler->OnKeyUp(KeyEvent);
}

FReply SImGuiWidgetEd::OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& AnalogInputEvent)
{
	return InputHandler->OnAnalogValueChanged(AnalogInputEvent);
}

FReply SImGuiWidgetEd::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	OnMouseMove(MyGeometry, MouseEvent);
	OnMouseEnter(MyGeometry, MouseEvent);
	OnMouseMove(MyGeometry, MouseEvent);
	//UE_LOG(LogTemp, Log, TEXT("%s %s"), TEXT("Down"), *MouseEvent.GetScreenSpacePosition().ToString());
	return InputHandler->OnMouseButtonDown(MouseEvent).LockMouseToWidget(SharedThis(this));
}

FReply SImGuiWidgetEd::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	OnMouseMove(MyGeometry, MouseEvent);
	return InputHandler->OnMouseButtonDoubleClick(MouseEvent).LockMouseToWidget(SharedThis(this));
}

bool NeedMouseLock(const FPointerEvent& MouseEvent)
{
#if FROM_ENGINE_VERSION(4, 20)
	return FSlateApplication::Get().GetPressedMouseButtons().Num() > 0;
#else
	return MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) || MouseEvent.IsMouseButtonDown(EKeys::MiddleMouseButton)
		|| MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton);
#endif
}

FReply SImGuiWidgetEd::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = InputHandler->OnMouseButtonUp(MouseEvent);
	//UE_LOG(LogTemp, Log, TEXT("%s %s"), TEXT("Up"), *MouseEvent.GetScreenSpacePosition().ToString());
	if (!NeedMouseLock(MouseEvent))
	{
		Reply.ReleaseMouseLock();
	}
	return Reply;
}

FReply SImGuiWidgetEd::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//UE_LOG(LogTemp, Log, TEXT("PPPPPPPInHo %f"), MouseEvent.GetWheelDelta());
	return InputHandler->OnMouseWheel(MouseEvent);
}

FReply SImGuiWidgetEd::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// const FSlateRenderTransform ImGuiToScreen = ImGuiTransform.Concatenate(MyGeometry.GetAccumulatedRenderTransform());
	const FSlateRenderTransform ImGuiToScreen = MyGeometry.GetAccumulatedRenderTransform();
	return InputHandler->OnMouseMove(ImGuiToScreen.Inverse().TransformPoint(MouseEvent.GetScreenSpacePosition()));
}

FReply SImGuiWidgetEd::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& FocusEvent)
{
	Super::OnFocusReceived(MyGeometry, FocusEvent);

	//IMGUI_WIDGET_LOG(VeryVerbose, TEXT("ImGui Widget %d - Focus Received."), ContextIndex);

	//bForegroundWindow = GameViewport->Viewport->IsForegroundWindow();
	InputHandler->OnKeyboardInputEnabled();
	InputHandler->OnGamepadInputEnabled();

	FSlateApplication::Get().ResetToDefaultPointerInputSettings();
	return FReply::Handled();
}

void SImGuiWidgetEd::OnFocusLost(const FFocusEvent& FocusEvent)
{
	Super::OnFocusLost(FocusEvent);

	//IMGUI_WIDGET_LOG(VeryVerbose, TEXT("ImGui Widget %d - Focus Lost."), ContextIndex);

	InputHandler->OnKeyboardInputDisabled();
	InputHandler->OnGamepadInputDisabled();
}

void SImGuiWidgetEd::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::OnMouseEnter(MyGeometry, MouseEvent);

	//IMGUI_WIDGET_LOG(VeryVerbose, TEXT("ImGui Widget %d - Mouse Enter."), ContextIndex);

	InputHandler->OnMouseInputEnabled();
}

void SImGuiWidgetEd::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::OnMouseLeave(MouseEvent);

	//IMGUI_WIDGET_LOG(VeryVerbose, TEXT("ImGui Widget %d - Mouse Leave."), ContextIndex);

	InputHandler->OnMouseInputDisabled();
}

void SImGuiWidgetEd::AddCharacter(TCHAR ch)
{
	if (InputHandler.IsValid() && InputHandler->GetInputState())
	{
		InputHandler->GetInputState()->AddCharacter(ch);
	}
}

void SImGuiWidgetEd::NotifyActiveImGuiInputText(ImGuiInputTextCallbackData* Data)
{
	if ((Data->EventFlag & ImGuiInputTextFlags_CallbackActive) != 0)
	{
		ImGuiContext* ImGuiContext = ImGui::GetCurrentContext();
		if (ImGuiContext)
		{
			ImGuiID ID = ImGui::GetActiveID();
			// UE_LOG(LogUnrealImGui, Log, TEXT("Active InputText %d"), ImGuiContext->ActiveId);
			CurrentActiveInputTextID = ImGuiContext->ActiveId;
			CurrentInputTextBuf = Data->Buf;
			CurrentActiveInputTextCallbackData = Data;
		}
		ITextInputMethodSystem* const TextInputMethodSystem = FSlateApplication::Get().GetTextInputMethodSystem();
		if (TextInputMethodSystem)
		{
			if (!bHasRegisteredTextInputMethodContext)
			{
				bHasRegisteredTextInputMethodContext = true;

				TextInputMethodChangeNotifier = TextInputMethodSystem->RegisterContext(TextInputMethodContext.ToSharedRef());
				if (TextInputMethodChangeNotifier.IsValid())
				{
					TextInputMethodChangeNotifier->NotifyLayoutChanged(ITextInputMethodChangeNotifier::ELayoutChangeType::Created);
				}
			}

			TextInputMethodContext->CacheWindow();
			TextInputMethodSystem->ActivateContext(TextInputMethodContext.ToSharedRef());
		}
	}
	else if ((Data->EventFlag & ImGuiInputTextFlags_CallbackDeactive) != 0)
	{
		CurrentInputTextBuf = nullptr;
		// CurrentData = nullptr;
	}
}

void SImGuiWidgetEd::OnPostImGuiUpdate()
{
}

FORCEINLINE FSlateRenderTransform RoundTranslation(const FSlateRenderTransform& Transform)
{
	const FVector2D& Translation = Transform.GetTranslation();
	return FSlateRenderTransform{ Transform.GetMatrix(),
		FVector2D{ FMath::RoundToFloat(Translation.X), FMath::RoundToFloat(Translation.Y) } };
}

int32 SImGuiWidgetEd::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, 
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& WidgetStyle, bool bParentEnabled) const
{
#if WITH_EDITOR
	//if (FImGuiContextProxy* ContextProxy = ModuleManager->GetContextManager().GetEditorContextProxy()) 
	{
		ModuleManager->GetContextManager().GetEditorWindowContextProxy(ContextIndex_UnOffset).Tick(FSlateApplication::Get().GetDeltaTime());

		const FSlateRenderTransform& WidgetToScreen = AllottedGeometry.GetAccumulatedRenderTransform();
		const FSlateRenderTransform ImGuiToScreen = RoundTranslation(ImGuiRenderTransform.Concatenate(WidgetToScreen));

		for (const auto& DrawList : ModuleManager->GetContextManager().GetEditorWindowContextProxy(ContextIndex_UnOffset).GetDrawData())
		{
			DrawList.CopyVertexData(VertexBuffer, ImGuiToScreen);
			int IndexBufferOffset = 0;
			for (int CommandNb = 0; CommandNb < DrawList.NumCommands(); CommandNb++)
			{
				const auto& DrawCommand = DrawList.GetCommand(CommandNb, ImGuiToScreen);

				DrawList.CopyIndexData(IndexBuffer, IndexBufferOffset, DrawCommand.NumElements);

				IndexBufferOffset += DrawCommand.NumElements;

				const FSlateResourceHandle& handle = ModuleManager->GetTextureManager().GetTextureHandle(DrawCommand.TextureId);

				const FSlateRect ClippingRect = DrawCommand.ClippingRect.IntersectionWith(MyClippingRect);

				OutDrawElements.PushClip(FSlateClippingZone{ ClippingRect });

				FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, handle, VertexBuffer, IndexBuffer, nullptr, 0, 0);

				OutDrawElements.PopClip();
			}
		}
	}
#endif
	return Super::OnPaint(Args, AllottedGeometry, MyClippingRect, OutDrawElements, LayerId, WidgetStyle, bParentEnabled);
}
