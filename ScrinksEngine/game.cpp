#include "game.h"
#include "node_model.h"
#include "pipeline.h"
#include "node_sprite.h"

using namespace scrinks::core;

void Game::init(std::unique_ptr<Node>&& r)
{
	s_root = std::move(r);

	s_spriteRenderer = std::make_shared<scrinks::render::SpriteRenderer>(render::TextureManager::load_and_store("mc", "assets/textures/tilemap_packed.png"), 12, 55000);
	s_spriteRenderer->init();

	auto sr = scrinks::render::Pipeline::get_pass<scrinks::render::pass::SpriteRender>("sprite_renderer");
	if (sr)
		sr->add_renderer(s_spriteRenderer);

	//// Grass
	//auto grass = s_root->new_child<nodes::DynamicModel>(render::ModelManager::load_and_store("grass", "D:/assets/voxel/big_crap_grass.obj"), Transform{});
	//grass->rename("grass");

	//// Balls
	//for (int x = 0; x < 10; x++)
	//{
	//	for (int z = 0; z < 10; z++)
	//	{
	//		auto sphere = s_root->new_child<nodes::DynamicModel>(render::ModelManager::load_and_store("sphere", "D:/assets/test/sphere.obj"), Transform{});
	//		sphere->rename("sphere[" + std::to_string(x) + "," + std::to_string(z) + "]");
	//		sphere->set_position(glm::vec3(x, 0.f, z));
	//		sphere->set_scale(0.5f);
	//		sphere->set_script(lua::ScriptManager::load_and_store("ball", "assets/scripts/ball.lua"));
	//	}
	//}

	// Test Sprites
	for (int y = 0; y < 100; y ++)
	{
		for (int x = 0; x < 100; x ++)
		{
			auto sprite = s_root->new_child<nodes::Sprite>(glm::linearRand(0.0f, 1024.0f), glm::vec2{x, y});
			sprite->rename("sprite[" + std::to_string(x) + "," + std::to_string(y) + "]");
			sprite->set_script(lua::ScriptManager::load_and_store("sprite", "assets/scripts/sprite.lua"));
		}
	}

	std::cout << "Sprites: " << s_spriteRenderer->m_count << std::endl;
}

void Game::update(float interpolate)
{
	nodes::Sprite::update_sprite_data(interpolate);
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

void Game::shutdown()
{
	s_root = nullptr;
}

std::pair<float, float> Game::mouse_pos()
{
	auto mp = ImGui::GetMousePos();
	return std::make_pair(mp.x, mp.y);
}

