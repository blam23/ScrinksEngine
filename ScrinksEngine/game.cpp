#include "game.h"
#include "node_model.h"

using namespace scrinks::core;

std::unique_ptr<Node> Game::s_root{ nullptr };

void scrinks::core::Game::init(std::unique_ptr<Node>&& r)
{
	s_root = std::move(r);
	//nodes::DynamicModel* n = new nodes::DynamicModel(s_root.get(), render::ModelManager::instance().load_and_store("knight", "D:/assets/voxel/chr_knight.obj"), {});
	//n->translate(glm::vec3(0, 2, 0));

	//n->rename("Printer");
	//n->set_script(lua::ScriptManager::instance().load_and_store("printy", "assets/scripts/printy.lua"));

	//s_root->reserve_child_nodes(1);
	//for (int i = 0; i < 1; i++)
	//{
	//	//nodes::DynamicModel* calcNode = new nodes::DynamicModel{ s_root.get(), render::ModelManager::instance().load_and_store("sphere", "D:/assets/primitives/sphere.dae"), {} };
	//	nodes::DynamicModel* calcNode = new nodes::DynamicModel{ s_root.get(), render::ModelManager::instance().load_and_store("gun", "assets/models/gun/gun.fbx"), {} };
	//	calcNode->rename("printy " + std::to_string(i));
	//	calcNode->set_script(lua::ScriptManager::instance().get("printy"));
	//	calcNode->set_visible(false);
	//}
	// 
	new nodes::DynamicModel(s_root.get(), render::ModelManager::instance().load_and_store("grass", "D:/assets/voxel/big_crap_grass.obj"), {});

	//nodes::DynamicModel* gun = new nodes::DynamicModel{ s_root.get(), render::ModelManager::instance().load_and_store("gun", "assets/models/gun/gun.fbx"), {} };
	//gun->rename("gun");
	//gun->set_script(lua::ScriptManager::instance().load_and_store("printy", "assets/scripts/printy.lua"));

	for (int x = 0; x < 10; x++)
	{
		for (int z = 0; z < 10; z++)
		{
			nodes::DynamicModel* sphere = new nodes::DynamicModel{ s_root.get(), render::ModelManager::instance().load_and_store("sphere", "D:/assets/test/sphere.obj"), {} };
			sphere->rename("sphere[" + std::to_string(x) + "," + std::to_string(z) + "]");
			sphere->set_position(glm::vec3(x, 0.f, z));
			sphere->set_scale(0.5f);
			sphere->set_script(lua::ScriptManager::instance().load_and_store("ball", "assets/scripts/ball.lua"));
		}
	}
}

void Game::sync_fixed_update()
{
	if (s_root)
		s_root->sync_fixed_update();
}

void Game::check_resources()
{
	if (!threads::on_window_thread())
		return;

	if (s_root)
		s_root->check_resources();
}

