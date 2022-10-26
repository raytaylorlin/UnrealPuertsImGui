#include "ImGuiTextInputMethodContext.h"
#include "imgui_internal.h"

TSharedRef<FImGuiTextInputMethodContext> FImGuiTextInputMethodContext::Create(const TSharedRef<SImGuiWidgetEd>& Widget)
{
	return MakeShareable(new FImGuiTextInputMethodContext(Widget));
}

void FImGuiTextInputMethodContext::CacheWindow()
{
	const TSharedRef<const SWidget> OwningSlateWidgetPtr = OwnerWidget.Pin().ToSharedRef();
	CachedParentWindow = FSlateApplication::Get().FindWidgetWindow(OwningSlateWidgetPtr);
}

bool FImGuiTextInputMethodContext::IsComposing()
{
	return bIsComposing;
}

bool FImGuiTextInputMethodContext::IsReadOnly()
{
	return false;
}

uint32 FImGuiTextInputMethodContext::GetTextLength()
{
	return CompositionString.Len();
}

void FImGuiTextInputMethodContext::GetSelectionRange(uint32& BeginIndex, uint32& Length, ECaretPosition& CaretPosition)
{
	BeginIndex = SelectionRangeBeginIndex;
	Length = SelectionRangeLength;
	CaretPosition = SelectionCaretPosition;
}

void FImGuiTextInputMethodContext::SetSelectionRange(const uint32 BeginIndex, const uint32 Length,
	const ECaretPosition CaretPosition)
{
	SelectionRangeBeginIndex = BeginIndex;
	SelectionRangeLength = Length;
	SelectionCaretPosition = CaretPosition;
}

void FImGuiTextInputMethodContext::GetTextInRange(const uint32 BeginIndex, const uint32 Length, FString& OutString)
{
	OutString = CompositionString.Mid(BeginIndex, Length);
}

void FImGuiTextInputMethodContext::SetTextInRange(const uint32 BeginIndex, const uint32 Length, const FString& InString)
{
	FString NewString;
	if (BeginIndex > 0)
	{
		NewString = CompositionString.Mid(0, BeginIndex);
	}

	NewString += InString;

	if ((int32)(BeginIndex + Length) < CompositionString.Len())
	{
		NewString += CompositionString.Mid(BeginIndex + Length, CompositionString.Len() - (BeginIndex + Length));
	}
	CompositionString = NewString;
}

int32 FImGuiTextInputMethodContext::GetCharacterIndexFromPoint(const FVector2D& Point)
{
	int32 ResultIdx = INDEX_NONE;
	return ResultIdx;
}

bool FImGuiTextInputMethodContext::GetTextBounds(const uint32 BeginIndex, const uint32 Length, FVector2D& Position,	FVector2D& Size)
{
	if (OwnerWidget.IsValid())
	{
		ImGuiContext* ImGuiContext = ImGui::GetCurrentContext();
		if (ImGuiContext)
		{
			Position = FVector2D(CachedGeometry.AbsolutePosition.X + ImGuiContext->PlatformImeLastPos.x,
				CachedGeometry.AbsolutePosition.Y + ImGuiContext->PlatformImeLastPos.y + 20);
		}
	}
	return false;
}

void FImGuiTextInputMethodContext::GetScreenBounds(FVector2D& Position, FVector2D& Size)
{
}

TSharedPtr<FGenericWindow> FImGuiTextInputMethodContext::GetWindow()
{
	const TSharedPtr<SWindow> SlateWindow = CachedParentWindow.Pin();
	return SlateWindow.IsValid() ? SlateWindow->GetNativeWindow() : nullptr;
}

void FImGuiTextInputMethodContext::BeginComposition()
{
	if (!bIsComposing)
	{
		bIsComposing = true;
	}
}

void FImGuiTextInputMethodContext::UpdateCompositionRange(const int32 InBeginIndex, const uint32 InLength)
{
	CompositionBeginIndex = InBeginIndex;
	CompositionLength = InLength;
}

void FImGuiTextInputMethodContext::EndComposition()
{
	if (bIsComposing)
	{
		bIsComposing = false;

		if (OwnerWidget.IsValid())
		{
			ImGuiContext* ImGuiContext = ImGui::GetCurrentContext();
			ImGuiID ID = OwnerWidget.Pin()->CurrentActiveInputTextID;
			if (ImGuiContext->InputTextState.ID == ID)
			{
				auto CharArray = CompositionString.GetCharArray();
				for (int i = 0; i < CharArray.Num(); i++)
				{
					OwnerWidget.Pin()->AddCharacter(CharArray[i]);
				}
				CompositionString.Empty();
				CompositionBeginIndex = 0;
				CompositionLength = 0;
				SelectionRangeBeginIndex = 0;
				SelectionRangeLength = 0;
			}
		}
	}
}

FImGuiTextInputMethodContext::FImGuiTextInputMethodContext(const TSharedRef<SImGuiWidgetEd>& Widget)
	: OwnerWidget(Widget)
	, bIsComposing(false)
	, CompositionBeginIndex(0)
	, CompositionLength(0)
	, SelectionRangeBeginIndex(0)
	, SelectionRangeLength(0)
	, SelectionCaretPosition(ECaretPosition::Beginning)
{
}
