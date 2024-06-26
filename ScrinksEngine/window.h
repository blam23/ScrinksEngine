#pragma once

#include <string>
#include <functional>
#include <memory>

#include "gl.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "gbuffer.h"
#include "camera.h"
#include "project.h"

namespace scrinks
{
	class Window
	{
	public:
		using FixedUpdateCallback = std::function<void()>;

		static bool init(int width, int height, const std::string& name, const std::string& projectFilePath);
		static void set_title(const std::string& title);
		static void setup_renderer(Project::Renderer renderer);
		static void shutdown();
		static void run_loop();
		static void set_vsync(bool enabled);
		static bool is_vsync_enabled();
		static void set_capture_cursor(bool capture);
		static void register_for_fixed_updates(FixedUpdateCallback func);
		static double fixed_updates_per_second();
		
		// If this is above noise levels we're in trouble
		static inline double last_script_await_time() { return s_lastScriptAwaitDuration; }

	public:
		static int s_windowWidth;
		static int s_windowHeight;

	private:
		static void handle_input();
		static float check_fixed_update_timer();
		static void fixed_update();
		static bool setup_imgui();
		static bool setup_glfw(int width, int height, const std::string& name);

		static GLFWwindow* s_window;
		static bool s_vsync;
		static bool s_inputActive;
		static GLenum s_log_level;

		static std::vector<FixedUpdateCallback> s_fixedUpdateCallbacks;
		static double s_lastScriptAwaitDuration;
	};
}
