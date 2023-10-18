#include "editor_debug.h"
#include "debug.h"
#include "window.h"
#include "shader.h"
#include "texture.h"
#include "model.h"
#include "gbuffer.h"
#include "pipeline.h"
#include "script.h"
#include "game.h"
#include "glm/glm.hpp"

#pragma warning(push)
#pragma warning( disable : 4244 )
#pragma warning( disable : 4100 )
#pragma warning( disable : 4305 )
#include "imGuIZMO.quat/imGuIZMOquat.h"
#pragma warning(pop)

using namespace scrinks;

int dbgTextureID{ 0 };

editor::AssetNameCache<render::TextureManager> textureNameCache{};
editor::AssetNameCache<render::BufferManager>  bufferNameCache{};
editor::AssetNameCache<render::ShaderManager>  shaderNameCache{};
editor::AssetNameCache<lua::ScriptManager>     scriptNameCache{};

float timeSinceRefresh = 0.0f;
int refreshIndex = 0;
void refresh_caches_as_needed(float interval = 100)
{
    ImGuiIO& io{ ImGui::GetIO() };

    timeSinceRefresh += io.DeltaTime;
    if (timeSinceRefresh < interval / 1000.0f)
        return;

    timeSinceRefresh = 0;

    switch (refreshIndex)
    {
        case 0: textureNameCache.refresh(); break;
        case 1: bufferNameCache.refresh(); break;
        case 2: shaderNameCache.refresh(); break;
        case 3: scriptNameCache.refresh(); break;
        default: refreshIndex = 0; break;
    }

    refreshIndex++;
}

float fov{ 90.0f };
void camera_info()
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

bool requestVsync{ true };
void debug()
{
    bool vsyncEnabled = Window::is_vsync_enabled();
    ImGui::Begin("Debug");
    {
        ImGui::Checkbox("Vsync", &requestVsync);
        if (requestVsync != vsyncEnabled)
        {
            Window::set_vsync(requestVsync);
        }

        if(ImGui::Button("Reload All"))
        {
            render::TextureManager::reload_all();
            render::ShaderManager::reload_all();
            render::ModelManager::reload_all();
            lua::ScriptManager::reload_all();
            core::Game::check_resources();
        }

        if (ImGui::Button("Reload Shaders"))
        {
            render::ShaderManager::reload_all();
            core::Game::check_resources();
        }

        if (ImGui::Button("Reload Scripts"))
        {
            lua::ScriptManager::reload_all();
            core::Game::check_resources();
        }

        if (ImGui::CollapsingHeader("Floats"))
        {
            for (auto& [name, value] : debug::get_all_test_floats())
            {
                ImGui::DragFloat(name.c_str(), &value, 0.01f);
                ImGui::Spacing();
            }
        }
        
        if (ImGui::CollapsingHeader("Directions"))
        {
            for (auto& [name, value] : debug::get_all_test_dirs())
            {
                ImGui::gizmo3D(name.c_str(), value);
                ImGui::Spacing();
            }
        }

        if (ImGui::CollapsingHeader("Colours"))
        {
            for (auto& [name, value] : debug::get_all_test_colours())
            {
                float col[3]{ value.r, value.g, value.b };
                ImGui::ColorEdit3(name.c_str(), col);
                ImGui::Spacing();
                value.r = col[0]; value.g = col[1]; value.b = col[2];
            }
        }
    }
    ImGui::End();
}

void assets()
{
    if (ImGui::Begin("Textures"))
    {
        static int chosen{ 1 };
        ImGui::PushItemWidth(-1);
        int count{ (int)textureNameCache.size() };
        static std::shared_ptr<render::Texture> displayTexture{ nullptr };
        if (count > 0)
        {
            chosen %= count;
            if (ImGui::BeginCombo("##", textureNameCache[chosen].data()))
            {
                int n{ 0 };
                for (const auto& name : textureNameCache)
                {
                    if (ImGui::Selectable(name.c_str(), n == chosen))
                    {
                        displayTexture = render::TextureManager::get(name);
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
            ImGui::Image((void*)(intptr_t)displayTexture->id(), ImVec2{ glm::min((float)displayTexture->width(), viewportPanelSize.x), glm::min((float)displayTexture->height(), viewportPanelSize.x) }, { 0, 1 }, { 1, 0 });
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
                        displayBuffer = render::BufferManager::get(name);
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
            ImGui::Image((void*)(intptr_t)displayBuffer->id(), ImVec2{ glm::min((float)displayBuffer->width(), viewportPanelSize.x), glm::min((float)displayBuffer->height(), viewportPanelSize.x) }, { 0, 1 }, { 1, 0 });
        }
    }
    ImGui::End();

    if (ImGui::Begin("Shaders"))
    {
        bool ignoreSelect;
        ImGui::PushItemWidth(-1);
        if (ImGui::BeginListBox("##hidelabel"))
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

    static float scriptTimes[512];
    static std::size_t scriptPtr = 0;
    scriptTimes[scriptPtr++] = (float)Window::last_script_await_time();
    scriptPtr %= 512;

    if (ImGui::Begin("Scripts"))
    {
        ImGui::PlotLines("Script Times", scriptTimes, 512);
        ImGui::Text("Last Await Time: %3.fms", Window::last_script_await_time() * 1000.f);

        bool ignoreSelect;
        ImGui::PushItemWidth(-1);
        if (ImGui::BeginListBox("##hidelabel"))
        {
            for (const auto& name : scriptNameCache)
            {
                ImGui::Selectable(name.c_str(), &ignoreSelect);
            }
            ImGui::EndListBox();
        }
        ImGui::PopItemWidth();
    }
    ImGui::End();
}

void editor::render_debug()
{
    refresh_caches_as_needed();
    camera_info();
    ::debug();
    assets();
}

