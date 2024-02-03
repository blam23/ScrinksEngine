#include "lua_engine.h"
#include "lua_bindings.h"
#include "window.h"
#include "game.h"
#include "node.h"

#include <iostream>

using namespace scrinks::lua;

thread_local sol::state mainState;
thread_local bool attemptedSetup;

scrinks::core::Node* currentNode{ nullptr };

void scrinks::lua::setup()
{
	mainState.open_libraries
	(
		sol::lib::base,
		sol::lib::package,
		sol::lib::math,
		sol::lib::string,
		sol::lib::table,
		sol::lib::bit32
	);

	const auto res{ mainState.do_file("lua/init.lua") };
	if (!res.valid())
	{
		sol::error err = res;
		std::cerr << "Failed to load init script: " << err.what() << std::endl;
	}
	else
	{
		std::cerr << "Loaded lua!" << std::endl;
	}

	bindings::setup_all(mainState);

	attemptedSetup = true;
}

sol::environment scrinks::lua::create_env()
{
	if (!attemptedSetup)
		setup();

	return { mainState, sol::create, mainState["_G"].tbl };
}

sol::load_result scrinks::lua::load(const std::string& code, const std::string& file)
{
	return mainState.load(code, file);
}

void scrinks::lua::dbg(sol::environment& env, const std::string& code)
{
	mainState.script(code, env);
}

void scrinks::lua::dbg_print_globals(sol::environment& env)
{
	mainState.script("for k,v in pairs(_G) do print(k,v) end", env);
}

sol::object scrinks::lua::copy_object(const sol::object& in, sol::environment& to)
{
	auto type = in.get_type();
	auto state = to.lua_state();

	switch (type)
	{
		case sol::type::string:   return sol::make_object(state, in.as<std::string>());
		case sol::type::number:   return sol::make_object(state, in.as<double>());
		case sol::type::boolean:  return sol::make_object(state, in.as<bool>());

		case sol::type::userdata:
		{
			if (in.is<glm::vec2>()) {
				const auto& in_vec{ in.as<glm::vec2>() };
				return sol::make_object<glm::vec2>(state, in_vec.x, in_vec.y);
			}
			return {};
		}

		case sol::type::table:
		{
			auto orig = in.as<sol::table>();
			auto clone = to.create();

			for (const auto& [k, v] : orig)
				clone.set(copy_object(k, to), copy_object(v, to));

			return clone;
		}

		// todo?
		case sol::type::function:
		case sol::type::lightuserdata:

		// ignore
		case sol::type::thread:
		case sol::type::none:
		case sol::type::lua_nil:
		case sol::type::poly:
		default:
			return {};
	}

	
}

