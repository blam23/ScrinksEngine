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

void scrinks::Window::register_for_fixed_updates(FixedUpdateCallback func)
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

    std::cout << "Running OpenGL " << major << "." << minor << " (" << glGetString(GL_VENDOR) << ")" << std::endl;
}

bool Window::setup_glfw(int width, int height, const std::string& name)
{
    glfwSetErrorCallback(errors::glfw_error_callback);

    if (!glfwInit())
    {
        std::cerr << "GLFW Init failed" << std::endl;
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
        std::cerr << "Failed to initialise OpenGL" << std::endl;
        return false;
    }

    if (!gl3wIsSupported(4, 3))
    {
        std::cerr << "Required OpenGL version 4.3 is not supported on this platform." << std::endl;
        return false;
    }

    print_gl_data();

    glfwSetWindowSizeCallback(s_window, window_size_callback);

    glDebugMessageCallback(errors::gl_print_error_callback, &s_log_level);
    glEnable(GL_DEBUG_OUTPUT);

    return true;
}

void setup_pipeline()
{
    // todo: get this from game config data
    render::default_2d_pipeline();
}

bool Window::init(int width, int height, const std::string& name)
{
    s_windowWidth = width;
    s_windowHeight = height;

    if (!setup_glfw(width, height, name))
    {
        std::cerr << "Couldn't create OpenGL context, exiting application." << std::endl;
        exit(1);
    }

    if (!setup_imgui())
    {
        std::cerr << "Failed to initialise GUI, exiting application." << std::endl;
        exit(2);
    }

    editor::init();

    setup_pipeline();

    core::Game::init("assets/scripts/game.lua");

    return true;
}


double scrinks::Window::s_lastScriptAwaitDuration{ 0 };
void scrinks::Window::fixed_update()
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
float scrinks::Window::check_fixed_update_timer()
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

double scrinks::Window::fixed_updates_per_second()
{
    double total = 0;
    for (double sample : avg)
        total += sample;

    return 1 / (total/avg.size());
}

void scrinks::Window::run_loop()
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

void scrinks::Window::shutdown()
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


