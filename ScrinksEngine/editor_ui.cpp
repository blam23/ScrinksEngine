#include "editor_ui.h"
#include "debug.h"
#include "imgui_style.h"
#include "window.h"
#include "shader.h"
#include "texture.h"
#include "model.h"
#include "gbuffer.h"
#include "pipeline.h"
#include "glm/glm.hpp"

using namespace scrinks;

bool requestVsync{ true };
ImColor info_regular{ ImColor{ 230, 230, 230 } };
ImColor info_highlight{ ImColor{ 120, 230, 230 } };
int dbgTextureID{ 0 };
bool unFocusGame{ true };

editor::AssetNameCache<render::TextureManager> textureNameCache{};
editor::AssetNameCache<render::BufferManager>  bufferNameCache{};
editor::AssetNameCache<render::ShaderManager>  shaderNameCache{};

float timeSinceRefresh = 0.0f;
int refreshIndex = 0;
void refresh_caches_as_needed(ImGuiIO& io, float interval = 100)
{
    timeSinceRefresh += io.DeltaTime;
    if (timeSinceRefresh < interval / 1000.0f)
        return;

    timeSinceRefresh = 0;

    switch (refreshIndex)
    {
        case 0: textureNameCache.refresh(); break;
        case 1: bufferNameCache.refresh(); break;
        case 2: shaderNameCache.refresh(); break;
        default: refreshIndex = 0; break;
    }

    refreshIndex++;
}

void render_viewport()
{
    std::shared_ptr<render::Buffer> viewport{ render::BufferManager::instance().get("viewport") };
        
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

            if (focused && unFocusGame)
                ImGui::SetWindowFocus("Debug");

            Window::set_input_active(focused);

            ImGui::Image((void*)(intptr_t)viewport->id(), ImVec2{ viewportPanelSize.x, viewportPanelSize.y });

            if (focused)
            {
                ImVec2 bl{ node->Pos.x + viewportPanelSize.x, node->Pos.y + viewportPanelSize.y };
                ImGui::GetForegroundDrawList()->AddRect(node->Pos, bl, IM_COL32(255, 0, 0, 200), 0, 0, 2.0f);
            }
        }
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

float fov{ 90.0f };
void show_camera_info()
{
    ImGui::Begin("Camera");
    {
        auto& camera = render::Pipeline::camera();

        if (ImGui::DragFloat("FoV", &fov))
            camera.set_fov(fov);

        ImGui::Text("pos { %.3f, %.3f, %.3f }", camera.pos().x, camera.pos().y, camera.pos().z);
        ImGui::Text("dir { %.3f, %.3f, %.3f }", camera.forward().x, camera.forward().y, camera.forward().z);
    }
    ImGui::End();
}


void editor::init()
{
    gui::set_style_large_mat_blue();

    debug::register_test_float("testA", 0.0f);
    debug::register_test_float("testB", 0.0f);
    debug::register_test_float("testC", 0.0f);
    debug::register_test_float("testD", 0.0f);
}

void editor::render_ui()
{
    bool vsyncEnabled = Window::is_vsync_enabled();
    auto& io = ImGui::GetIO();

    unFocusGame = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Escape);

    refresh_caches_as_needed(io);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    render_viewport();

    ImGui::BeginMainMenuBar();
    {
        if (ImGui::MenuItem("Reload All"))
        {
            render::ShaderManager::instance().reload_all();
            render::ModelManager::instance().reload_all();
            render::TextureManager::instance().reload_all();

            render::Pipeline::force_recreate();
        }

        if (ImGui::MenuItem("Reload Shaders"))
        {
            render::ShaderManager::instance().reload_all();

            render::Pipeline::force_recreate();
        }

        ImGui::SameLine(ImGui::GetWindowWidth() - 235);
        ImGui::TextColored(vsyncEnabled ? info_regular : info_highlight, "%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::EndMainMenuBar();
    }

    show_camera_info();

    ImGui::Begin("Debug");
    {
        for (auto& [name, value] : debug::get_all_test_floats())
        {
            ImGui::DragFloat(name.c_str(), &value);
            ImGui::Spacing();
        }
    }
    ImGui::End();

    ImGui::Begin("Sidebar");
    {
        ImGui::Checkbox("Vsync", &requestVsync);
        if (requestVsync != vsyncEnabled)
        {
            Window::set_vsync(requestVsync);
        }
    }
    ImGui::End();

    if(ImGui::Begin("Textures"))
    {
        static int chosen{ 1 };
        ImGui::PushItemWidth(-1);
        int count{ (int)textureNameCache.size() };
        static std::shared_ptr<render::Texture> displayTexture{ nullptr };
        if(count > 0)
        {
            chosen %= count;
            if (ImGui::BeginCombo("##", textureNameCache[chosen].data()))
            {
                int n{ 0 };
                for (const auto& name : textureNameCache)
                {
                    if (ImGui::Selectable(name.c_str(), n == chosen))
                    {
                        displayTexture = render::TextureManager::instance().get(name);
                        chosen = n;
                    }
                    n++;
                }
                ImGui::EndCombo();
            }
        }
        ImGui::PopItemWidth();

        if (displayTexture)
        {
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            ImGui::Image((void*)(intptr_t)displayTexture->id(), ImVec2{ glm::min((float)displayTexture->width(), viewportPanelSize.x), glm::min((float)displayTexture->height(), viewportPanelSize.x) });
        }
    }
    ImGui::End();

    if (ImGui::Begin("Buffers"))
    {
        static int chosen{ 1 };
        static std::shared_ptr<render::Buffer> displayBuffer{ nullptr };
        ImGui::PushItemWidth(-1);
        int count{ (int)bufferNameCache.size() };
        if (count > 0)
        {
            chosen %= count;
            if (ImGui::BeginCombo("##", bufferNameCache[chosen].data()))
            {
                int n{ 0 };
                for (const auto& name : bufferNameCache)
                {
                    if (ImGui::Selectable(name.c_str(), n == chosen))
                    {
                        displayBuffer = render::BufferManager::instance().get(name);
                        chosen = n;
                    }
                    n++;
                }
                ImGui::EndCombo();
            }
        }
        ImGui::PopItemWidth();

        if (displayBuffer)
        {
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            ImGui::Image((void*)(intptr_t)displayBuffer->id(), ImVec2{ glm::min((float)displayBuffer->width(), viewportPanelSize.x), glm::min((float)displayBuffer->height(), viewportPanelSize.x )});
        }
    }
    ImGui::End();

    if (ImGui::Begin("Shaders"))
    {
        bool ignoreSelect;
        ImGui::PushItemWidth(-1);
        if(ImGui::BeginListBox("##hidelabel"))
        {
            for (const auto& name : shaderNameCache)
            {
                ImGui::Selectable(name.c_str(), &ignoreSelect);
            }
            ImGui::EndListBox();
        }
        ImGui::PopItemWidth();
    }
    ImGui::End();

    ImGui::Render();

    glViewport(0, 0, Window::s_windowWidth, Window::s_windowHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGui::EndFrame();
    ImGui::UpdatePlatformWindows();
}

