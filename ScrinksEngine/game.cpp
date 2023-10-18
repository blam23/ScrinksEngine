#include "game.h"
#include "node_model.h"

using namespace scrinks::core;

std::unique_ptr<Node> Game::s_root{ nullptr };

void scrinks::core::Game::init(std::unique_ptr<Node>&& r)
{
	s_root = std::move(r);

	// Grass
	auto grass = s_root->new_child<nodes::DynamicModel>(render::ModelManager::load_and_store("grass", "D:/assets/voxel/big_crap_grass.obj"), Transform{});
	grass->rename("grass");

	// Balls
	for (int x = 0; x < 10; x++)
	{
		for (int z = 0; z < 10; z++)
		{
			auto sphere = s_root->new_child<nodes::DynamicModel>(render::ModelManager::load_and_store("sphere", "D:/assets/test/sphere.obj"), Transform{});
			sphere->rename("sphere[" + std::to_string(x) + "," + std::to_string(z) + "]");
			sphere->set_position(glm::vec3(x, 0.f, z));
			sphere->set_scale(0.5f);
			sphere->set_script(lua::ScriptManager::load_and_store("ball", "assets/scripts/ball.lua"));
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

void scrinks::core::Game::shutdown()
{
	s_root = nullptr;
}

