#include "editor_debug.h"
#include "debug.h"
#include "window.h"
#include "shader.h"
#include "texture.h"
#include "model.h"
#include "gbuffer.h"
#include "pipeline.h"
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

        for (auto& [name, value] : debug::get_all_test_floats())
        {
            ImGui::DragFloat(name.c_str(), &value);
            ImGui::Spacing();
        }
        for (auto& [name, value] : debug::get_all_test_dirs())
        {
            ImGui::gizmo3D(name.c_str(), value);
            ImGui::Spacing();
        }
    }
    ImGui::End();
}

render::pass::ShadowMap* shadowMap{ nullptr };
void editor::add_shadow_map_tracker(render::pass::ShadowMap* m)
{
    shadowMap = m;
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
}

void editor::render_debug()
{
    refresh_caches_as_needed();
    camera_info();
    ::debug();
    assets();
}

