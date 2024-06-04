#include "window.h"

#include "editor_main.h"
#include "game.h"
#include "debug.h"
#include "errors.h"
#include "default_pipeline.h"
#include "thread_pool.h"

#include <iostream>
#include <array>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "spdlog/spdlog.h"

using namespace scrinks;

bool Window::s_vsync{ true };
bool Window::s_inputActive{ false };
GLenum Window::s_log_level{ GL_DEBUG_SEVERITY_LOW };
int Window::s_windowWidth { 1920 };
int Window::s_windowHeight{ 1080 };
GLFWwindow* Window::s_window{ nullptr };

std::vector<Window::FixedUpdateCallback> Window::s_fixedUpdateCallbacks{};

static void window_size_callback(GLFWwindow*, int width, int height)
{
    Window::s_windowWidth = width;
    Window::s_windowHeight = height;
}

void Window::set_capture_cursor(bool capture)
{
    glfwSetInputMode(s_window, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Window::register_for_fixed_updates(FixedUpdateCallback func)
{
    s_fixedUpdateCallbacks.push_back(func);
}


bool Window::setup_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontFromFileTTF("assets/fonts/open-sans-semi.ttf", 20);
    ImGui_ImplGlfw_InitForOpenGL(s_window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    return true;
}

void print_gl_data()
{
    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    spdlog::info("Running OpenGL {}.{} ({})", major, minor, (const char*)glGetString(GL_VENDOR));
}

bool Window::setup_glfw(int width, int height, const std::string& name)
{
    glfwSetErrorCallback(errors::glfw_error_callback);

    if (!glfwInit())
    {
        spdlog::error("GLFW Init failed");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    s_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (s_window == nullptr)
        return false;
    glfwMakeContextCurrent(s_window);
    set_vsync(false);

    if (gl3wInit())
    {
        spdlog::error("Failed to initialise OpenGL");
        return false;
    }

    if (!gl3wIsSupported(4, 3))
    {
        spdlog::error("Required OpenGL version 4.3 is not supported on this platform");
        return false;
    }

    print_gl_data();

    glfwSetWindowSizeCallback(s_window, window_size_callback);

    glDebugMessageCallback(errors::gl_print_error_callback, &s_log_level);
    glEnable(GL_DEBUG_OUTPUT);

    return true;
}

void Window::setup_renderer(Project::Renderer renderer)
{
    if (renderer == Project::Renderer::Sprite)
    {
        render::default_2d_pipeline();
    }
    else
    {
        render::default_pipeline();
    }
}

bool Window::init(int width, int height, const std::string& name, const std::string& projectFilePath)
{
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<threads::thread_pool_formatter>('*').set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%*] [%^%l%$] %v");
    spdlog::set_formatter(std::move(formatter));

    threads::setup();

    s_windowWidth = width;
    s_windowHeight = height;

    if (!setup_glfw(width, height, name))
    {
        spdlog::error("Couldn't create OpenGL context, exiting application");
        exit(1);
    }

    if (!setup_imgui())
    {
        spdlog::error("Failed to initialise GUI, exiting application");
        exit(2);
    }

    editor::init();

    if (!core::Game::init(projectFilePath))
    {
        spdlog::error("Failed to initialise game, exiting application");
        return false;
    }

    threads::dispatch_async([] (void*) { lua::setup(); lua::load_classes(); }, true);

    return true;
}

void Window::set_title(const std::string& title)
{
    glfwSetWindowTitle(s_window, title.c_str());
}

double Window::s_lastScriptAwaitDuration{ 0 };
void Window::fixed_update()
{
    double pre = glfwGetTime();
    threads::await_previous();
    core::Game::sync_fixed_update();
    threads::dispatch_async(&core::Node::dispatch_fixed_update);
    s_lastScriptAwaitDuration = glfwGetTime() - pre;
}

std::array<double,20> avg{ 0.0 };
int slide = 0;
double fixedUpdatePrev{ 0 };
double fixedUpdateDelta{ 0 };
int nextFixedUpdate{ 0 };
float Window::check_fixed_update_timer()
{
    std::uint16_t loops{ 0 };
    while ((glfwGetTime() * 1000) > nextFixedUpdate && loops < core::Game::MaxFrameSkip)
    {
        fixed_update();
        
        fixedUpdateDelta = glfwGetTime() - fixedUpdatePrev;
        fixedUpdatePrev = glfwGetTime();

        avg[slide] = fixedUpdateDelta;
        slide++;
        slide %= avg.size();

        nextFixedUpdate += core::Game::SkipRate;
        loops++;
    }

    return (float)((glfwGetTime() * 1000) + core::Game::SkipRate - nextFixedUpdate) / (float)(core::Game::SkipRate);
}

double Window::fixed_updates_per_second()
{
    double total = 0;
    for (double sample : avg)
        total += sample;

    return 1 / (total/avg.size());
}

void Window::run_loop()
{
    while (!glfwWindowShouldClose(s_window))
    {
        glfwPollEvents();

        float interpolation{ check_fixed_update_timer() };
        interpolation = glm::clamp(interpolation, 0.0f, 1.0f);

        core::Game::update(interpolation);
        render::Pipeline::draw(interpolation);

        editor::render_ui();

        glfwSwapBuffers(s_window);
    }
}

void Window::shutdown()
{
    threads::shutdown();
    scrinks::core::Game::shutdown();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(s_window);
    glfwTerminate();
}

void Window::set_vsync(bool enabled)
{
    s_vsync = enabled;
    glfwSwapInterval(enabled ? 1 : 0);
}

bool Window::is_vsync_enabled()
{
	return s_vsync;
}
