#include "editor_scene_tree.h"
#include "node.h"
#include "game.h"

#include "inspector_node3d.h"

#include <string>
#include <set>
#include <map>

using namespace scrinks;

core::Node* sceneNode{ nullptr };
bool initTree{ false };
std::map<std::string, editor::NodeInspectorFunc> inspectorMap{};

void setup_tree()
{
	sceneNode = core::Game::root();

	editor::register_node_inspector("Model", editor::node_model_inspector);
	editor::register_node_inspector("Node3D", editor::node_3d_inspector);
	editor::register_node_inspector("Node2D", editor::node_2d_inspector);
	editor::register_node_inspector("Sprite", editor::node_sprite_inspector);
}

void editor::register_node_inspector(const std::string& typeName, NodeInspectorFunc func)
{
	inspectorMap.emplace(typeName, func);
}

static std::set<core::Node*> selected{};
core::Node* build_tree_for_node(core::Node& in, size_t& i, core::Node* parentOfNew)
{
	static ImGuiTreeNodeFlags baseFlags{ ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding };

	core::Node* clicked{ nullptr };

	for (const auto& node : in.children())
	{
		if (!node)
			continue;

		ImGuiTreeNodeFlags flags = baseFlags;

		bool isSelected{ selected.find(node) != selected.end() };
		if (isSelected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2260494828224182f, 0.3559437096118927f, 0.446351945400238f, 1.0f));
		}

		if (parentOfNew == node)
			ImGui::SetNextItemOpen(true);

		if (node->children().size() == 0)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool nodeOpen = ImGui::TreeNodeEx((void*)node->id(), flags, node->name());

		if (isSelected)
		{
			ImGui::PopStyleColor();
		}

		if (ImGui::IsItemClicked())
			clicked = node;

		if (node->children().size() > 0)
		{
			if (nodeOpen)
			{
				core::Node* out{ build_tree_for_node(*node, i, parentOfNew) };

				if (out != nullptr)
					clicked = out;

				ImGui::TreePop();
			}
		}
		else
		{
			ImGui::SameLine(10);
			ImGui::Indent();
			ImGui::Text("-");
			ImGui::Unindent();
			//ImGui::GetForegroundDrawList()->AddNgon(ImGui::GetCursorPos(), 5.f, 0xFFFFFFFF, 4, 2.0f);
		}

	}

	return clicked;
}


void editor::scene_tree()
{
	if (!initTree)
		setup_tree();

	if (!sceneNode)
		return;

	core::Node* clicked{ nullptr };

	ImGui::Begin("Scene");
	{
		core::Node* parentOfNew{ nullptr };
		if (ImGui::Button("Add") && selected.size() == 1)
		{
			core::Node* parent{ *selected.begin() };
			core::Node* newChild{ new core::Node{ parent } };

			selected.clear();
			selected.insert(newChild);
			parentOfNew = parent;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0, 2.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextBorderSize, 0);
		ImGui::BeginChild("NodeList");
		{

			size_t nodes{ 0 };

			clicked = build_tree_for_node(*sceneNode, nodes, parentOfNew);
			
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

			if (clicked)
			{
				//if (ImGui::GetIO().KeyCtrl)
				//{
				//	if (selected.find(clicked) == selected.end())
				//		selected.insert(clicked);
				//	else
				//		selected.erase(clicked);
				//}
				//else
				//{
					selected.clear();
					selected.insert(clicked);
				//}
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();

	ImGui::Begin("Inspector");
	{
		if (selected.size() == 1)
		{
			static std::string nameInput;

			core::Node& node{ **selected.begin() };

			if (clicked)
				nameInput = clicked->name();

			ImGui::SeparatorText("Core");
			ImGui::InputString("Name: ", &nameInput);

			if (nameInput.compare(node.name()) != 0)
			{
				ImGui::SameLine();
				if (ImGui::Button("Apply"))
					node.rename(nameInput);

				ImGui::SameLine();
				if (ImGui::Button("X"))
					nameInput = node.name();
			}

			// Draw all the inspectors for the given node
			std::string type{ node.type() };
			while (!type.empty())
			{
				const auto inspector{ inspectorMap.find(type) };
				if (inspector != inspectorMap.end())
				{
					ImGui::SeparatorText(type.c_str());
					type = inspector->second(&node);
				}
				else
					break;
			}
		}
	}
	ImGui::End();
}
