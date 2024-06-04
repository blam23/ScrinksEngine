#pragma once

#include "gl.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include <functional>
#include <string>


namespace scrinks::editor
{
	using WindowFunc = std::function<void()>;

	void init();
	void render_ui();

	void register_window_func(WindowFunc* const func);
}

namespace ImGui
{
	int StringResizeCallback(ImGuiInputTextCallbackData* data);
	bool InputString(const char* label, std::string* str, ImGuiInputTextFlags flags = 0);
}
