#pragma once

#include "node_2d.h"
#include "node_3d.h"
#include "node_model.h"
#include "node_sprite.h"
#include "editor_scene_tree.h"

#pragma warning(push)
#pragma warning( disable : 4244 )
#pragma warning( disable : 4100 )
#pragma warning( disable : 4305 )
#include "imGuIZMO.quat/imGuIZMOquat.h"
#pragma warning(pop)

namespace scrinks::editor
{
	std::string node_3d_inspector(core::Node* in)
	{
		core::Node3D* node{ (core::Node3D*)in};

		ImGui::Text("Position"); ImGui::SameLine(100);
		ImGui::SliderFloat3("##pos", glm::value_ptr(node->pos()), -5.f, 5.f);

		ImGui::Text("Scale"); ImGui::SameLine(100);
		ImGui::SliderFloat3("##scale", glm::value_ptr(node->scale()), 0.f, 2.f);

		ImGui::Text("Rot"); ImGui::SameLine(100);
		ImGui::SliderFloat4("##rot", glm::value_ptr(node->rot()), 0.f, 1.f);

		//static glm::quat rot = node->rot();
		//if (ImGui::gizmo3D("Rotation", rot))
		//{
		//	node->set_rotation(rot);
		//	std::cout << "Rot: [" << rot.x << "," << rot.y << "," << rot.z << "," << rot.w << "]" << std::endl;
		//}

		// Return base class
		return "Node";
	}

	std::string node_model_inspector(core::Node* in)
	{
		core::nodes::DynamicModel* node{ (core::nodes::DynamicModel*)in};

		ImGui::Checkbox("Visible", &node->visible());

		return "Node3D";
	}

	std::string node_2d_inspector(core::Node* in)
	{
		core::Node2D* node{ (core::Node2D*)in };

		ImGui::Text("Position"); ImGui::SameLine(100);
		ImGui::SliderFloat2("##pos", glm::value_ptr(node->pos()), -5.f, 5.f);

		ImGui::Text("Rot"); ImGui::SameLine(100);
		ImGui::SliderFloat("##rot", node->rot_ptr(), -glm::half_pi<float>(), glm::half_pi<float>());

		return "Node";
	}

	std::string node_sprite_inspector(core::Node* in)
	{
		core::nodes::Sprite* node{ (core::nodes::Sprite*)in };

		ImGui::SliderFloat("Tile Idx", node->tile_index_ptr(), 0, 1024, "%.0f");

		return "Node2D";
	}

}

