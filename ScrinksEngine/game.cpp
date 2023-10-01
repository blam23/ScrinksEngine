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

	s_root->reserve_child_nodes(10000);
	for (int i = 0; i < 10000; i++)
	{
		nodes::DynamicModel* calcNode = new nodes::DynamicModel{ s_root.get(), render::ModelManager::instance().get("knight"), {} };
		calcNode->rename("printy " + std::to_string(i));
		calcNode->set_script(lua::ScriptManager::instance().get("printy"));
		calcNode->set_visible(false);
	}
	 
	new nodes::DynamicModel(s_root.get(), render::ModelManager::instance().load_and_store("grass", "D:/assets/voxel/monu1.obj"), {});
}

void Game::fixed_update()
{
	if (s_root)
		s_root->fixed_update();
}

void Game::check_resources()
{
	if (!threads::on_window_thread())
		return;

	if (s_root)
		s_root->check_resources();
}

