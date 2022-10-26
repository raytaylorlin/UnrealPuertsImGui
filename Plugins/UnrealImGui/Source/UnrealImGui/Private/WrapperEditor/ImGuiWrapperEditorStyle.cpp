// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ImGuiWrapperEditorStyle.h"

#include "EditorStyleSet.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FImGuiWrapperEditorStyle::StyleInstance = NULL;

void FImGuiWrapperEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FImGuiWrapperEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FImGuiWrapperEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ImGuiWrapperEditorStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FImGuiWrapperEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ImGuiWrapperEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("UnrealImGui")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ImGuiWrapperEditor.OpenPluginWindow", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), Icon40x40));

	Style->Set("ImGuiWrapperEditor.Tab", FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		.SetUncheckedPressedImage(BOX_BRUSH("PlacementMode/TabActive", 8.0f / 32.0f))
		.SetCheckedImage(BOX_BRUSH("PlacementMode/TabActive", 8.0f / 32.0f))
		.SetCheckedHoveredImage(BOX_BRUSH("PlacementMode/TabActive", 8.0f / 32.0f))
		.SetCheckedPressedImage(BOX_BRUSH("PlacementMode/TabActive", 8.0f / 32.0f))
		.SetPadding(0));

	Style->Set("ZPlacement.Asset", FButtonStyle()
		.SetNormal(FSlateNoResource())
		.SetHovered(IMAGE_BRUSH("Common/Selection", FVector2D(8, 8), FLinearColor(1.0f, 1.0f, 1.0f, 0.1f)))
		//.SetPressed(IMAGE_BRUSH("Common/Selection", FVector2D(8, 8), SelectionColor))
		.SetNormalPadding(0)
		.SetPressedPadding(0)
	);

	const int32 FontSize = 12;
	const FTextBlockStyle NormalText = FEditorStyle::GetWidgetStyle<FTextBlockStyle>("NormalText");
	FTextBlockStyle BlackFont = FTextBlockStyle(NormalText)
		//.SetColorAndOpacity(FSlateColor::UseSubduedForeground())
		;
	{
		BlackFont.Font.Size = FontSize - 3;
		BlackFont.SetColorAndOpacity(FSlateColor(FLinearColor::Black));
		Style->Set("BlackFont", BlackFont);
	}

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FImGuiWrapperEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const TSharedPtr< FSlateStyleSet >  FImGuiWrapperEditorStyle::Get()
{
	return StyleInstance;
}


FLinearColor DimBackground = FLinearColor(FColor(64, 64, 64));
FLinearColor DimBackgroundHover = FLinearColor(FColor(50, 50, 50));
FLinearColor DarkBackground = FLinearColor(FColor(42, 42, 42));
