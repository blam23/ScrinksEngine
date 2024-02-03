#include "editor_main.h"

#include "Window.h"
#include "imgui_style.h"
#include "pipeline.h"
#include "editor_debug.h"
#include "editor_scene_tree.h"
#include "game.h"

#include <vector>

using namespace scrinks;

bool drawDebug{ false };
bool dbgKeyReleased{ false };

bool viewportFocused{ false };
bool viewportActive{ false };
bool viewportJustActive{ false };

std::vector<editor::WindowFunc*> windowCallbacks{};

glm::vec2 oldMousePos{ 0.0f, 0.0f };
float mouseMaxMove{ 100.0f };
void handle_input()
{
    ImGuiIO& io = ImGui::GetIO();

    render::FPSCamera& camera{ render::Pipeline::camera() };

    glm::vec2 newMousePos = glm::vec2{ io.MousePos.x, io.MousePos.y };
    glm::vec2 mouseDelta = oldMousePos - newMousePos;
    float deltaLen = glm::length(mouseDelta);
    oldMousePos = newMousePos;

    if (viewportJustActive)
        viewportJustActive = false;
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

ImColor info_regular{ ImColor{ 230, 230, 230 } };
ImColor info_highlight{ ImColor{ 120, 230, 230 } };
void menu()
{
    //const ImGuiViewport* viewport = ImGui::GetMainViewport();

    //// Set position to the bottom of the viewport
    //ImGui::SetNextWindowPos(
    //    ImVec2(viewport->Pos.x,
    //        viewport->Pos.y + viewport->Size.y - ImGui::GetFrameHeight()));

    //// Extend width to viewport width
    //ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, ImGui::GetFrameHeight()));

    //// Add menu bar flag and disable everything else
    //ImGuiWindowFlags flags =
    //    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
    //    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse |
    //    ImGuiWindowFlags_NoSavedSettings |
    //    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground |
    //    ImGuiWindowFlags_MenuBar;

    bool vsyncEnabled = Window::is_vsync_enabled();
    auto& io = ImGui::GetIO();

    ImGui::BeginMainMenuBar();
    {
        ImGui::Text("Scrinks");

        if (ImGui::Button("Reload Scripts"))
        {
            lua::ScriptManager::reload_all();
            core::Game::check_resources();
        }

        ImGui::SameLine(ImGui::GetWindowWidth() - 295);
        ImGui::TextColored(vsyncEnabled ? info_regular : info_highlight, "%.3f ms/frame (%.1f FPS) [%.1f fixed]", 1000.0f / io.Framerate, io.Framerate, Window::fixed_updates_per_second());
    }
    ImGui::EndMainMenuBar();
}

void check_inputs()
{
    if (viewportActive)
    {
        handle_input();
    }

    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_F11))
    {
        if (dbgKeyReleased)
        {
            drawDebug = !drawDebug;
            dbgKeyReleased = false;
        }
    }
    else
        dbgKeyReleased = true;
}

void viewport()
{
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Escape))
        viewportActive = false;

    std::shared_ptr<render::Buffer> viewport{ render::BufferManager::get("sprites") };

    ImGuiID id = ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode, nullptr);
    ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(id);

    ImGuiWindowClass centralAlways = {};
    centralAlways.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe;
    ImGui::SetNextWindowClass(&centralAlways);
    ImGui::SetNextWindowDockID(node->ID, ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

    ImGui::Begin("Viewport", nullptr);
    {
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        render::Pipeline::resize_if_needed((int)viewportPanelSize.x, (int)viewportPanelSize.y);

        if (viewport && viewport->id() > 0)
        {
            const bool focused{ ImGui::IsWindowFocused() };

            if (!viewportActive && focused && !viewportFocused && !ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Escape))
            {
                viewportJustActive = true;
                viewportActive = true;
                viewportFocused = true;
                ImGui::SetWindowFocus();
            }

            viewportFocused = focused;

            ImGui::Image((void*)(intptr_t)viewport->id(), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, { 0, 1 }, { 1, 0 });

            if (viewportActive && drawDebug)
            {
                ImVec2 viewRect{ node->Pos.x + viewportPanelSize.x, node->Pos.y + viewportPanelSize.y };
                ImGui::GetForegroundDrawList()->AddRect(node->Pos, viewRect, IM_COL32(0, 255, 200, 255), 0, 0, 2.0f);
            }
        }
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void editor::init()
{
    set_style();
}

void editor::render_ui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        viewport();

        check_inputs();

        if (drawDebug)
        {
            scene_tree();
            render_debug();
        }

        menu();

        //ImGui::ShowDemoWindow();

        for (const auto func : windowCallbacks)
            (*func)();
    }
    ImGui::Render();

    glViewport(0, 0, Window::s_windowWidth, Window::s_windowHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGui::EndFrame();
    ImGui::UpdatePlatformWindows();
}

void editor::register_window_func(WindowFunc* const func)
{
    windowCallbacks.push_back(func);
}

int ImGui::StringResizeCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        std::string* str{ (std::string*)data->UserData };
        IM_ASSERT(str->data() == data->Buf);
        str->reserve(data->BufSize);
        str->resize(data->BufTextLen);
        data->Buf = str->data();
    }
    return 0;
}

bool ImGui::InputString(const char* label, std::string* str, ImGuiInputTextFlags flags)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);

    ImGui::AlignTextToFramePadding();
    ImGui::Text(label);

    ImGui::SameLine();
    ImGui::PushItemWidth(200);
    bool ret
    {
        ImGui::InputText("##_name_input", str->data(), str->size() + 1, flags | ImGuiInputTextFlags_CallbackResize, StringResizeCallback, (void*)str)
    };
    ImGui::PopItemWidth();

    return ret;
}
