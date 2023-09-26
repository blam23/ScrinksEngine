#include "editor_ui.h"
#include "debug.h"
#include "imgui_style.h"
#include "deferred_renderer.h"
#include "shader.h"
#include "texture.h"
#include "gbuffer.h"
#include "glm/glm.hpp"

using namespace scrinks;

bool requestVsync{ true };
ImColor info_regular{ ImColor{ 230, 230, 230 } };
ImColor info_highlight{ ImColor{ 120, 230, 230 } };
int dbgTextureID{ 0 };

std::vector<std::string> textureNameCache{};
std::vector<std::string> bufferNameCache{};
std::vector<std::string> shaderNameCache{};

void render_viewport(GLuint renderedSceneTexture)
{
    ImGuiID id = ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode, nullptr);
    ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(id);

    ImGuiWindowClass centralAlways = {};
    centralAlways.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe;
    ImGui::SetNextWindowClass(&centralAlways);
    ImGui::SetNextWindowDockID(node->ID, ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

    ImGui::Begin("Viewport", nullptr);
    {
        if (renderedSceneTexture > 0)
        {
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            ImGui::Image((void*)(intptr_t)renderedSceneTexture, ImVec2{ viewportPanelSize.x, viewportPanelSize.y });

            Renderer::resize_view_if_needed((int)viewportPanelSize.x, (int)viewportPanelSize.y);
        }
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void editor::init()
{
    gui::set_style_material_blue();

    debug::register_test_float("testA", 0.0f);
    debug::register_test_float("testB", 0.0f);
    debug::register_test_float("testC", 0.0f);
    debug::register_test_float("testD", 0.0f);
}

void editor::render_ui(GLuint renderedSceneTexture)
{
    bool vsyncEnabled = Renderer::is_vsync_enabled();
    auto& io = ImGui::GetIO();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    render_viewport(renderedSceneTexture);

    ImGui::BeginMainMenuBar();
    {
        if (ImGui::MenuItem("Reload"))
        {
           render::ShaderManager::instance().reload_all();
        }

        if (ImGui::BeginMenu("Menu"))
        {
            if (ImGui::MenuItem("Quit", "alt+f4"))
                exit(0);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Theme"))
        {
            if (ImGui::MenuItem("Dark"))
                gui::set_style_dark();

            if (ImGui::MenuItem("Blue"))
                gui::set_style_material_blue();

            ImGui::EndMenu();
        }

        ImGui::SameLine(ImGui::GetWindowWidth() - 235);
        ImGui::TextColored(vsyncEnabled ? info_regular : info_highlight, "%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Debug");
    {
        for (auto& [name, value] : debug::get_all_test_floats())
        {
            ImGui::SliderFloat(name.c_str(), &value, -2.f, 2.0f);
            ImGui::Spacing();
        }
    }
    ImGui::End();

    ImGui::Begin("Sidebar");
    {
        ImGui::Checkbox("Vsync", &requestVsync);
        if (requestVsync != vsyncEnabled)
        {
            Renderer::set_vsync(requestVsync);
        }
    }
    ImGui::End();

    if(ImGui::Begin("Textures"))
    {
        if (ImGui::Button("Refresh"))
        {
            textureNameCache.clear();
            render::TextureManager::instance().for_each(
                [] (const std::string name, std::shared_ptr<render::Texture>) -> void
                {
                    textureNameCache.push_back(name);
                }
            );
        }

        static int chosen{ 1 };
        ImGui::PushItemWidth(-1);
        int count{ (int)textureNameCache.size() };
        static std::shared_ptr<render::Texture> displayTexture{ nullptr };
        if(count > 0 && ImGui::BeginCombo("##", textureNameCache[chosen].data()))
        {
            if (chosen < count)
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
            }
            ImGui::EndCombo();
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
        if (ImGui::Button("Refresh"))
        {
            bufferNameCache.clear();
            render::BufferManager::instance().for_each(
                [] (const std::string name, std::shared_ptr<render::Buffer>) -> void
                {
                    bufferNameCache.push_back(name);
                }
            );
        }

        static int chosen{ 1 };
        static std::shared_ptr<render::Buffer> displayBuffer{ nullptr };
        ImGui::PushItemWidth(-1);
        int count{ (int)bufferNameCache.size() };
        if (count > 0 && ImGui::BeginCombo("##", bufferNameCache[chosen].data()))
        {
            if (chosen < count)
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
            }
            ImGui::EndCombo();
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
        if (ImGui::Button("Refresh"))
        {
            shaderNameCache.clear();
            render::ShaderManager::instance().for_each(
                [](const std::string name, std::shared_ptr<render::Shader>) -> void
                {
                    shaderNameCache.push_back(name);
                }
            );
        }

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

    glViewport(0, 0, Renderer::s_windowWidth, Renderer::s_windowHeight);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGui::EndFrame();
    ImGui::UpdatePlatformWindows();
}

