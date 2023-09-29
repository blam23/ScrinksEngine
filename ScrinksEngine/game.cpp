#include "game.h"
#include "node_model.h"

using namespace scrinks::core;

std::unique_ptr<Node> Game::s_root{ nullptr };

void scrinks::core::Game::init(std::unique_ptr<Node>&& r)
{
	s_root = std::move(r);
	nodes::DynamicModel* n = new nodes::DynamicModel(s_root.get(), render::ModelManager::instance().load_and_store("torus", "assets/models/torus.obj"), {});
	n->translate(glm::vec3(0, 2, 0));
	n = new nodes::DynamicModel(s_root.get(), render::ModelManager::instance().load_and_store("torus", "assets/models/torus.obj"), {});
	n->translate(glm::vec3(-2, 2, -2));
	new nodes::DynamicModel(s_root.get(), render::ModelManager::instance().load_and_store("room", "assets/models/room.obj"), {});
}

void Game::fixed_update()
{
	if (s_root)
		s_root->fixed_update();
}

