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

namespace scrinks
{
	class Renderer
	{
	public:
		using ViewResizeCallback = std::function<void(int width, int height)>;

		static bool init(int width, int height, const std::string& name);
		static void shutdown();
		static void run_loop();
		static void set_vsync(bool enabled);
		static bool is_vsync_enabled();
		static void set_resize_callback(ViewResizeCallback callback);
		static void resize_view(int width, int height);
		static void resize_view_if_needed(int currentWidth, int currentHeight);

	public:
		static int s_windowWidth;
		static int s_windowHeight;
		static int s_viewWidth;
		static int s_viewHeight;

	private:
		static void handle_resize();
		static bool setup_imgui();
		static bool setup_glfw(int width, int height, const std::string& name);
		static void render_gbuffer();
		static void render_scene();

		static GLFWwindow* s_window;
		static bool s_vsync;
		static bool s_resized;
		static ViewResizeCallback s_viewResizeCallback;
		static std::unique_ptr<render::GBuffer> s_gBuffer;
		static GLenum s_log_level;
	};
}
