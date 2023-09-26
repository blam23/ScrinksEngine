#pragma once

#include <gl/gl3w.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

namespace scrinks::editor
{
	void init();
	void render_ui(GLuint renderedSceneTexture);
}
