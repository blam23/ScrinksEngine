#include "lua_engine.h"
#include "window.h"

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

