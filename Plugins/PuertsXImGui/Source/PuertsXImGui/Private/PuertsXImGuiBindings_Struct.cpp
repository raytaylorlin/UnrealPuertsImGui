// Generated by gen_imgui_puerts_bindings.py

#include "PuertsXImGui.h"
#include "UnrealImGuiModule.h"
#include "Binding.hpp"

UsingCppType(ImVec2);
UsingCppType(ImVec4);
UsingCppType(ImGuiStyle);

struct AutoRegisterForCPP_PuertsXImGuiBindings_Struct
{
    AutoRegisterForCPP_PuertsXImGuiBindings_Struct()
    {
        puerts::DefineClass<ImVec2>()
            .Constructor(CombineConstructors(
                MakeConstructor(ImVec2, float, float),
                MakeConstructor(ImVec2)
            ))
            .Property("x", MakeProperty(&ImVec2::x))
            .Property("y", MakeProperty(&ImVec2::y))
            .Register();
        puerts::DefineClass<ImVec4>()
            .Constructor(CombineConstructors(
                MakeConstructor(ImVec4, float, float, float, float),
                MakeConstructor(ImVec4)
            ))
            .Property("x", MakeProperty(&ImVec4::x))
            .Property("y", MakeProperty(&ImVec4::y))
            .Property("z", MakeProperty(&ImVec4::z))
            .Property("w", MakeProperty(&ImVec4::w))
            .Register();
        puerts::DefineClass<ImGuiStyle>()
            .Property("Alpha", MakeProperty(&ImGuiStyle::Alpha))
            .Property("DisabledAlpha", MakeProperty(&ImGuiStyle::DisabledAlpha))
            .Property("WindowPadding", MakeProperty(&ImGuiStyle::WindowPadding))
            // .Property("WindowRounding", MakeProperty(&ImGuiStyle::))
            // .Property("WindowBorderSize", MakeProperty(&ImGuiStyle::))
            // .Property("WindowMinSize", MakeProperty(&ImGuiStyle::))
            // .Property("WindowTitleAlign", MakeProperty(&ImGuiStyle::))
            // .Property("WindowMenuButtonPosition", MakeProperty(&ImGuiStyle::))
            // .Property("ChildRounding", MakeProperty(&ImGuiStyle::))
            // .Property("ChildBorderSize", MakeProperty(&ImGuiStyle::))
            // .Property("PopupRounding", MakeProperty(&ImGuiStyle::))
            // .Property("PopupBorderSize", MakeProperty(&ImGuiStyle::))
            // .Property("FramePadding", MakeProperty(&ImGuiStyle::))
            // .Property("FrameRounding", MakeProperty(&ImGuiStyle::))
            // .Property("FrameBorderSize", MakeProperty(&ImGuiStyle::))
            // .Property("ItemSpacing", MakeProperty(&ImGuiStyle::))
            // .Property("ItemInnerSpacing", MakeProperty(&ImGuiStyle::))
            // .Property("CellPadding", MakeProperty(&ImGuiStyle::))
            // .Property("TouchExtraPadding", MakeProperty(&ImGuiStyle::))
            // .Property("IndentSpacing", MakeProperty(&ImGuiStyle::))
            // .Property("ColumnsMinSpacing", MakeProperty(&ImGuiStyle::))
            // .Property("ScrollbarSize", MakeProperty(&ImGuiStyle::))
            // .Property("ScrollbarRounding", MakeProperty(&ImGuiStyle::))
            // .Property("GrabMinSize", MakeProperty(&ImGuiStyle::))
            // .Property("GrabRounding", MakeProperty(&ImGuiStyle::))
            // .Property("LogSliderDeadzone", MakeProperty(&ImGuiStyle::))
            // .Property("TabRounding", MakeProperty(&ImGuiStyle::))
            // .Property("TabBorderSize", MakeProperty(&ImGuiStyle::))
            // .Property("TabMinWidthForCloseButton", MakeProperty(&ImGuiStyle::))
            // .Property("ColorButtonPosition", MakeProperty(&ImGuiStyle::))
            // .Property("ButtonTextAlign", MakeProperty(&ImGuiStyle::))
            // .Property("SelectableTextAlign", MakeProperty(&ImGuiStyle::))
            // .Property("DisplayWindowPadding", MakeProperty(&ImGuiStyle::))
            // .Property("DisplaySafeAreaPadding", MakeProperty(&ImGuiStyle::))
            // .Property("MouseCursorScale", MakeProperty(&ImGuiStyle::))
            // .Property("AntiAliasedLines", MakeProperty(&ImGuiStyle::))
            // .Property("AntiAliasedLinesUseTex", MakeProperty(&ImGuiStyle::))
            // .Property("AntiAliasedFill", MakeProperty(&ImGuiStyle::))
            // .Property("CurveTessellationTol", MakeProperty(&ImGuiStyle::))
            // .Property("CircleTessellationMaxError", MakeProperty(&ImGuiStyle::))
            // .Property("Colors", MakeProperty(&ImGuiStyle:Colors))
            .Register();

        
        // ['ImVec2', 'ImVec4', 'ImVector', 'ImGuiStyle', 'ImGuiIO', 'ImGuiInputTextCallbackData', 'ImGuiSizeCallbackData', 'ImGuiPayload', 'ImGuiTableColumnSortSpecs', 'ImGuiTableSortSpecs', 'ImGuiOnceUponAFrame', 'ImGuiTextFilter', 'ImGuiTextRange', 'ImGuiTextBuffer', 'ImGuiStorage', 'ImGuiStoragePair', 'ImGuiListClipper', 'ImColor', 'ImDrawCmd', 'ImDrawVert', 'ImDrawCmdHeader', 'ImDrawChannel', 'ImDrawListSplitter', 'ImDrawList', 'ImDrawData', 'ImFontConfig', 'ImFontGlyph', 'ImFontGlyphRangesBuilder', 'ImFontAtlasCustomRect', 'ImFontAtlas', 'ImFont', 'ImGuiViewport']
    }
};

AutoRegisterForCPP_PuertsXImGuiBindings_Struct _AutoRegisterForCPP_PuertsXImGuiBindings_Struct__;