#pragma once

#include "imgui.h"

namespace scrinks::editor
{
	void set_style()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Alpha = 1.0f;
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ColorButtonPosition = ImGuiDir_Left;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.FramePadding = ImVec2(16.0f, 5.0f);
		style.ItemSpacing = ImVec2(8.0f, 8.0f);
		style.ItemInnerSpacing = ImVec2(1.0f, 1.0f);
		style.SeparatorTextAlign = ImVec2(0.5f, 0.5f);
		style.SeparatorTextBorderSize = 1;
		style.TabRounding = 0.0f;
		style.ChildRounding = 0.0f;
		style.WindowRounding = 0.0f;
		style.IndentSpacing = 9.0f;
		style.GrabMinSize = 20.0f;
		style.Colors[ImGuiCol_ChildBg] = style.Colors[ImGuiCol_TabUnfocused];
		style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Tab];

		//style.Alpha = 1.0f;
		//style.DisabledAlpha = 0.6000000238418579f;
		//style.WindowRounding = 0.0f;
		//style.WindowBorderSize = 1.0f;
		//style.WindowMinSize = ImVec2(32.0f, 32.0f);
		//style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		//style.ChildRounding = 0.0f;
		//style.ChildBorderSize = 1.0f;
		//style.PopupRounding = 2.200000047683716f;
		//style.PopupBorderSize = 1.0f;
		//style.FrameRounding = 0.0f;
		//style.FrameBorderSize = 1.0f;
		//style.ItemSpacing = ImVec2(7.0f, 7.0f);
		//style.ItemInnerSpacing = ImVec2(7.0f, 7.099999904632568f);
		//style.CellPadding = ImVec2(6.0f, 6.0f);
		//style.IndentSpacing = 6.0f;
		//style.ColumnsMinSpacing = 6.0f;
		//style.ScrollbarSize = 13.19999980926514f;
		//style.ScrollbarRounding = 1.200000047683716f;
		//style.GrabMinSize = 10.0f;
		//style.GrabRounding = 0.0f;
		//style.TabRounding = 0.0f;
		//style.TabBorderSize = 0.0f;
		//style.TabMinWidthForCloseButton = 0.0f;
		//style.ColorButtonPosition = ImGuiDir_Right;
		//style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		//style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		//style.Colors[ImGuiCol_Text] = ImVec4(0.9527801871299744f, 0.9527897238731384f, 0.9527801871299744f, 1.0f);
		//style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5193111300468445f, 0.5193133354187012f, 0.5193081498146057f, 1.0f);
		//style.Colors[ImGuiCol_WindowBg] = ImVec4(0.130621612071991f, 0.1294737309217453f, 0.1416308879852295f, 1.0f);
		//style.Colors[ImGuiCol_PopupBg] = ImVec4(0.04139385372400284f, 0.04313725605607033f, 0.04110726714134216f, 1.0f);
		//style.Colors[ImGuiCol_Border] = ImVec4(9.999921530834399e-07f, 9.999999974752427e-07f, 9.999899930335232e-07f, 0.2188841104507446f);
		//style.Colors[ImGuiCol_BorderShadow] = ImVec4(9.999899930335232e-07f, 9.999999974752427e-07f, 9.999947678807075e-07f, 0.1287553906440735f);
		//style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
		//style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
		//style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
		//style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04139385372400284f, 0.04313725605607033f, 0.04110726714134216f, 1.0f);
		//style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 1.0f);
		//style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		//style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
		//style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
		//style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
		//style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f, 0.5400000214576721f);
		//style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.4435033202171326f, 0.716738224029541f, 0.3968207240104675f, 1.0f);
		//style.Colors[ImGuiCol_CheckMark] = ImVec4(0.4435033202171326f, 0.716738224029541f, 0.3968207240104675f, 1.0f);
		//style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
		//style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
		//style.Colors[ImGuiCol_Button] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
		//style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
		//style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
		//style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
		//style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.3600000143051147f);
		//style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 0.3300000131130219f);
		//style.Colors[ImGuiCol_Separator] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
		//style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
		//style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
		//style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
		//style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
		//style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
		//style.Colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
		//style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
		//style.Colors[ImGuiCol_TabActive] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.3600000143051147f);
		//style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
		//style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
		//style.Colors[ImGuiCol_PlotLines] = ImVec4(0.613383948802948f, 0.7939914464950562f, 0.6753950715065002f, 1.0f);
		//style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.4435033202171326f, 0.716738224029541f, 0.3968207240104675f, 1.0f);
		//style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.613383948802948f, 0.7939914464950562f, 0.6753950715065002f, 1.0f);
		//style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.4435033202171326f, 0.716738224029541f, 0.3968207240104675f, 1.0f);
		//style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
		//style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
		//style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
		//style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		//style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
		//style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
		//style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.3294117748737335f, 0.6666666865348816f, 0.8588235378265381f, 1.0f);
		//style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.613383948802948f, 0.7939914464950562f, 0.6753950715065002f, 1.0f);
		//style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.4435033202171326f, 0.716738224029541f, 0.3968207240104675f, 1.0f);
		//style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.613383948802948f, 0.7939914464950562f, 0.6753950715065002f, 0.442060112953186f);
		//style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.613383948802948f, 0.7939914464950562f, 0.6753950715065002f, 0.442060112953186f);
	}
}
