#pragma once

#include <vector>
#include <string>

#include <gl/gl3w.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

namespace scrinks::editor
{
    template <typename T_Manager>
    class AssetNameCache : public std::vector<std::string>
    {
    public:
        void refresh()
        {
            std::vector<std::string>::clear();
            T_Manager::instance().for_each_name
            (
                [&] (const std::string name) -> void
                {
                    std::vector<std::string>::push_back(name);
                }
            );
        }
    };

	void init();
	void render_ui();
}
