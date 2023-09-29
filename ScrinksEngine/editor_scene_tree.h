#pragma once

#include "editor_main.h"
#include "node.h"
#include <functional>

namespace scrinks::editor
{
	using NodeInspectorFunc = std::function<std::string(core::Node*)>;
	void register_node_inspector(const std::string& typeName, NodeInspectorFunc);

	void scene_tree();
}

