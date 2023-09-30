#include "game.h"
#include "node_model.h"

using namespace scrinks::core;

std::unique_ptr<Node> Game::s_root{ nullptr };

void scrinks::core::Game::init(std::unique_ptr<Node>&& r)
{
	s_root = std::move(r);
	nodes::DynamicModel* n = new nodes::DynamicModel(s_root.get(), render::ModelManager::instance().load_and_store("knight", "D:/assets/voxel/chr_knight.obj"), {});
	n->translate(glm::vec3(0, 2, 0));

	n->rename("Printer");
	n->set_script(lua::ScriptManager::instance().load_and_store("printy", "assets/scripts/printy.lua"));

	s_root->reserve_child_nodes(1000);
	for (int i = 0; i < 1000; i++)
	{
		Node* calcNode = new core::Node{ s_root.get() };
		calcNode->rename("Calcer " + std::to_string(i));
		calcNode->set_script(lua::ScriptManager::instance().load_and_store("calc", "assets/scripts/calc.lua"));
	}
	 
	new nodes::DynamicModel(s_root.get(), render::ModelManager::instance().load_and_store("grass", "D:/assets/voxel/monu1.obj"), {});
}

void Game::fixed_update()
{
	if (s_root)
		s_root->fixed_update();
}

