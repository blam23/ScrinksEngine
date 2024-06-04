#include "game.h"
#include "node_model.h"
#include "pipeline.h"
#include "node_sprite.h"
#include "node_root.h"
#include "spdlog/spdlog.h"

using namespace scrinks::core;

bool Game::init(const std::string_view projectFilePath)
{
	s_root = std::make_unique<nodes::Root>();
	auto project = Project::from_file(projectFilePath);
	if (!project)
	{
		spdlog::error("Failed to initialise project, exiting game");
		return false;
	}

	Window::setup_renderer(project->renderer());
	Window::set_title(project->name());

	lua::load_classes();
	s_root->set_script(project->entry_script());

	s_spriteRenderer = std::make_shared<scrinks::render::SpriteRenderer>(render::TextureManager::load_and_store("mc", "examples/space/textures/space.png"), 16, 300'000);
	s_spriteRenderer->init();

	auto sr = scrinks::render::Pipeline::get_pass<scrinks::render::pass::SpriteRender>("sprite_renderer");
	if (sr)
		sr->add_renderer(s_spriteRenderer);

	s_root->start_game();

	const auto children = s_root->get_children_of_type<nodes::Root>();

	for (const auto child : children)
	{
		spdlog::info("Child: {}", (void*)child);
	}

	return true;
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


