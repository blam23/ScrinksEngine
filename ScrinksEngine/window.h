#pragma once

#include <string>
#include <functional>
#include <memory>

#include <gl/gl3w.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "gbuffer.h"
#include "camera.h"

namespace scrinks
{
	class Window
	{
	public:
		static bool init(int width, int height, const std::string& name);
		static void shutdown();
		static void run_loop();
		static void set_vsync(bool enabled);
		static bool is_vsync_enabled();

		static void set_input_active(bool active);

	public:
		static int s_windowWidth;
		static int s_windowHeight;

	private:
		static void handle_input();
		static bool setup_imgui();
		static bool setup_glfw(int width, int height, const std::string& name);

		static GLFWwindow* s_window;
		static bool s_vsync;
		static bool s_inputActive;
		static GLenum s_log_level;
	};
}
