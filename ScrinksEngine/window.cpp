#include "window.h"

#include "editor_ui.h"
#include "debug.h"
#include "errors.h"
#include "default_pipeline.h"

#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

using namespace scrinks;

bool Window::s_vsync{ true };
bool Window::s_inputActive{ false };
GLenum Window::s_log_level{ GL_DEBUG_SEVERITY_LOW };
int Window::s_windowWidth { 0 };
int Window::s_windowHeight{ 0 };
GLFWwindow* Window::s_window{ nullptr };

static void window_size_callback(GLFWwindow*, int width, int height)
{
    Window::s_windowWidth = width;
    Window::s_windowHeight = height;
}

bool Window::setup_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontFromFileTTF("assets/fonts/fira.otf", 16);
    ImGui_ImplGlfw_InitForOpenGL(s_window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    return true;
}

bool Window::setup_glfw(int width, int height, const std::string& name)
{
    glfwSetErrorCallback(errors::glfw_error_callback);

    if (!glfwInit())
    {
        std::cerr << "GLFW Init failed" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    s_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (s_window == nullptr)
        return false;
    glfwMakeContextCurrent(s_window);
    set_vsync(true);

    if (gl3wInit())
    {
        std::cerr << "Failed to initialise OpenGL" << std::endl;
        return false;
    }

    if (!gl3wIsSupported(3, 2))
    {
        std::cerr << "Required OpenGL version 3.2 is not supported on this platform." << std::endl;
        return false;
    }

    glfwSetWindowSizeCallback(s_window, window_size_callback);

    glDebugMessageCallback(errors::gl_print_error_callback, &s_log_level);
    glEnable(GL_DEBUG_OUTPUT);

    return true;
}

void setup_pipeline()
{
    render::default_pipeline();
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

    setup_pipeline();

    return true;
}


bool oldActive{ false };
bool justActivated{ true };
void scrinks::Window::set_input_active(bool active)
{
    if (!oldActive)
    {
        justActivated = true;
        oldActive = true;
    }

    s_inputActive = active;
    glfwSetInputMode(s_window, GLFW_CURSOR, active ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

glm::vec2 oldMousePos{ 0.0f, 0.0f };
float mouseMaxMove{ 100.0f };
void Window::handle_input()
{
    ImGuiIO& io = ImGui::GetIO();

    render::FPSCamera& camera{ render::Pipeline::camera() };

    glm::vec2 newMousePos = glm::vec2{ io.MousePos.x, io.MousePos.y };
    glm::vec2 mouseDelta = oldMousePos - newMousePos;
    float deltaLen = glm::length(mouseDelta);
    oldMousePos = newMousePos;

    if (justActivated)
        justActivated = false;
    else
    {
        if (deltaLen > mouseMaxMove)
            mouseDelta = glm::normalize(mouseDelta) * mouseMaxMove;

        camera.process_mouse(mouseDelta.x, mouseDelta.y, io.DeltaTime);
    }

    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W))
    {
        camera.move_relative_to_target(io.DeltaTime, render::Direction::Forward);
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_A))
    {
        camera.move_relative_to_target(io.DeltaTime, render::Direction::Left);
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_S))
    {
        camera.move_relative_to_target(io.DeltaTime, render::Direction::Backward);
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_D))
    {
        camera.move_relative_to_target(io.DeltaTime, render::Direction::Right);
    }
}

void scrinks::Window::run_loop()
{
    while (!glfwWindowShouldClose(s_window))
    {
        glfwPollEvents();

        if (s_inputActive)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            handle_input();
        }
        oldActive = s_inputActive;

        render::Pipeline::draw();
        editor::render_ui();

        glfwSwapBuffers(s_window);
    }
}

void scrinks::Window::shutdown()
{
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


