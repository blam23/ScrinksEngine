#include "lua_bindings.h"
#include "game.h"
#include "pipeline.h"
#include "window.h"
#include "node_sprite.h"
#include "glm/gtx/string_cast.hpp"

using namespace scrinks;

static void setup_game_bindings(sol::state& env)
{
	env["fixed_delta"] = 1.0f / core::Game::TickRate;

	env["mouse_position"]
		= [] () { return core::Game::mouse_pos(); };

	env["view_size"]
		= [] () { return std::pair(render::Pipeline::view_width(), render::Pipeline::view_height()); };

	env["is_key_down"]
		= [] (int key) { return ImGui::IsKeyDown((ImGuiKey)key); };

	env["create_node"]
		= [] () { return core::Game::root()->new_child<core::Node>(); };

	env["create_sprite_node"]
		= [] (float tileIdx, float x, float y) { return core::Game::root()->new_child<core::nodes::Sprite>(tileIdx, x, y); };

	env["root_node"]
		= [] () { return (core::Node*)core::Game::root(); };

	env["get_nodes_in_group"]
		= [] (const std::string& group) { return core::Node::get_typed_nodes_in_group<core::Node2D>(group); };

	env.new_usertype<core::Node>("base",
		"rename", &core::Node::rename,
		"script", &core::Node::set_script,
		"set_and_load_script", &core::Node::set_and_load_script,
		"add_to_group", &core::Node::add_to_group,
		"property", sol::overload(& core::Node::set_property, &core::Node::get_property),
		"mark_for_deletion", &core::Node::mark_for_deletion);

	env.new_usertype<core::Node2D>("node2D",
		"position", sol::overload(&core::Node2D::set_position, &core::Node2D::get_position),
		"translate", &core::Node2D::translate,
		sol::base_classes, sol::bases<core::Node>());

	env.new_usertype<core::nodes::Sprite>("sprite",
		"tile_index", sol::overload(&core::nodes::Sprite::set_tile_index, &core::nodes::Sprite::get_tile_index),
		"new", [] (float tileIdx, float x, float y) { return core::Game::root()->new_child<core::nodes::Sprite>(tileIdx, x, y); },
		sol::base_classes, sol::bases<core::Node2D, core::Node>());

	auto vec_mult_overloads = sol::overload(
		[] (const glm::vec2& a, const glm::vec2& b) -> glm::vec2 { return a * b; },
		[] (const glm::vec2& a, float b) -> glm::vec2 { return a * b; },
		[] (float a, const glm::vec2& b) -> glm::vec2 { return a * b; }
	);

	auto vec_add_overloads = sol::overload(
		[] (const glm::vec2& a, const glm::vec2& b) -> glm::vec2 { return a + b; },
		[] (const glm::vec2& a, float b) -> glm::vec2 { return a + b; },
		[] (float a, const glm::vec2& b) -> glm::vec2 { return a + b; }
	);

	auto vec_sub_overloads = sol::overload(
		[] (const glm::vec2& a, const glm::vec2& b) -> glm::vec2 { return a - b; },
		[] (const glm::vec2& a, float b) -> glm::vec2 { return a - b; },
		[] (float a, const glm::vec2& b) -> glm::vec2 { return a - b; }
	);

	auto vec_div_overloads = sol::overload(
		[] (const glm::vec2& a, const glm::vec2& b) -> glm::vec2 { return a / b; },
		[] (const glm::vec2& a, float b) -> glm::vec2 { return a / b; },
		[] (float a, const glm::vec2& b) -> glm::vec2 { return a / b; }
	);

	env.new_usertype<glm::vec2>
	(
		"vec2",
		
		"new", sol::constructors<void(float, float)>(),
		"x", &glm::vec2::x,
		"y", &glm::vec2::y,
		"normalize", [] (const glm::vec2& in) { return glm::normalize(in); },
		"length", [] (const glm::vec2& in) { return glm::length(in); },
		"length2", [] (const glm::vec2& in) { return glm::length2(in); },
		sol::meta_function::multiplication, vec_mult_overloads,
		sol::meta_function::addition, vec_add_overloads,
		sol::meta_function::subtraction, vec_sub_overloads,
		sol::meta_function::division, vec_div_overloads,
		sol::meta_function::to_string, [] (const glm::vec2& in) { return glm::to_string(in); }
	);
}

static bool registered = lua::bindings::register_binding(&setup_game_bindings);
