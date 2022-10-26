#pragma once

class SImGuiWidgetEd;

class FImGuiTextInputMethodContext : public ITextInputMethodContext
{
public:
	static TSharedRef<FImGuiTextInputMethodContext> Create(const TSharedRef<SImGuiWidgetEd>& Widget);
	void CacheWindow();

	bool UpdateCachedGeometry(const FGeometry& InAllottedGeometry)
	{
		if (CachedGeometry != InAllottedGeometry)
		{
			CachedGeometry = InAllottedGeometry;
			return true;
		}
		return false;
	}

	virtual bool IsComposing() override;
	virtual bool IsReadOnly() override;
	virtual uint32 GetTextLength() override;
	virtual void GetSelectionRange(uint32& BeginIndex, uint32& Length, ECaretPosition& CaretPosition) override;
	virtual void SetSelectionRange(const uint32 BeginIndex, const uint32 Length, const ECaretPosition CaretPosition) override;
	virtual void GetTextInRange(const uint32 BeginIndex, const uint32 Length, FString& OutString) override;
	virtual void SetTextInRange(const uint32 BeginIndex, const uint32 Length, const FString& InString) override;
	virtual int32 GetCharacterIndexFromPoint(const FVector2D& Point) override;
	virtual bool GetTextBounds(const uint32 BeginIndex, const uint32 Length, FVector2D& Position, FVector2D& Size) override;
	virtual void GetScreenBounds(FVector2D& Position, FVector2D& Size) override;
	virtual TSharedPtr<FGenericWindow> GetWindow() override;
	virtual void BeginComposition() override;
	virtual void UpdateCompositionRange(const int32 InBeginIndex, const uint32 InLength) override;
	virtual void EndComposition() override;

private:
	FImGuiTextInputMethodContext(const TSharedRef<SImGuiWidgetEd>& Widget);
	TWeakPtr<SImGuiWidgetEd> OwnerWidget;
	TWeakPtr<SWindow> CachedParentWindow;
	FGeometry CachedGeometry;

	bool bIsComposing;
	int32 CompositionBeginIndex;
	uint32 CompositionLength;
	uint32 SelectionRangeBeginIndex;
	uint32 SelectionRangeLength;
	ECaretPosition SelectionCaretPosition;
	FString CompositionString;
};
