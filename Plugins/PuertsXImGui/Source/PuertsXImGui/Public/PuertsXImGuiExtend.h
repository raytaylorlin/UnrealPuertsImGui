// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UnrealImGuiModule.h"

namespace PuertsXImGui
{
	inline void Text(const char* fmt) { ImGui::Text(fmt); }
	inline void TextColored(const ImVec4& col, const char* fmt) { ImGui::TextColored(col, fmt); }
	inline void TextDisabled(const char* fmt) { ImGui::TextDisabled(fmt); }
	inline void TextWrapped(const char* fmt) { ImGui::TextWrapped(fmt); }
	inline void LabelText(const char* label, const char* fmt) { ImGui::LabelText(label, fmt); }
	inline void BulletText(const char* fmt) { ImGui::BulletText(fmt); }
	
    inline bool TreeNode(const char* str_id, const char* fmt) { return ImGui::TreeNode(str_id, fmt); }
    inline bool TreeNodeEx(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt) { return ImGui::TreeNodeEx(str_id, flags, fmt); }
    inline void SetTooltip(const char* fmt) { ImGui::SetTooltip(fmt); }

	// For triggering UE IME
	inline int ImGuiInputTextCallback_UE(ImGuiInputTextCallbackData *data)
	{
		FUnrealImGuiModule::Get().NotifyActiveImGuiInputText(data);
		return 0;
	}
	
	inline bool InputText(const char* label, std::string* buf, ImGuiInputTextFlags flags = 0)
	{
		if (ImGui::BeginTable("UnrealPuertsImGui_InputText_WithIME", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 150.0f); ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableNextRow(); ImGui::TableNextColumn(); ImGui::TextUnformatted(label);
			ImGui::TableNextColumn();
			bool result = ImGui::InputText("", buf, flags | ImGuiInputTextFlags_Callback, ImGuiInputTextCallback_UE);
			ImGui::EndTable();
			return result;
		}
		return false;
	}
	
	inline bool InputTextMultiline(const char* label, std::string* buf, const ImVec2& size = ImVec2(0,0), ImGuiInputTextFlags flags = 0)
	{
		return ImGui::InputTextMultiline(label, buf, size, flags | ImGuiInputTextFlags_Callback, ImGuiInputTextCallback_UE);
	}

	inline bool InputTextWithHint(const char* label, const char* hint, std::string* buf, ImGuiInputTextFlags flags = 0)
	{
		if (ImGui::BeginTable("imgui_puerts_extend_InputTextWithHint_WithIME", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 150.0f); ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableNextRow(); ImGui::TableNextColumn(); ImGui::TextUnformatted(label);
			ImGui::TableNextColumn();
			bool result = ImGui::InputTextWithHint("", hint, buf, flags | ImGuiInputTextFlags_Callback, ImGuiInputTextCallback_UE);
			ImGui::EndTable();
			return result;
		}
		return false;
	}
}
