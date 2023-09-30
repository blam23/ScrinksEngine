#include "lua_engine.h"
#include "window.h"

#include "node.h"

#include <iostream>

using namespace scrinks::lua;

static sol::state mainState;
static scrinks::core::Node* currentNode{ nullptr };

bool scrinks::lua::setup()
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

	return true;
}

sol::environment scrinks::lua::create_env()
{
	return { mainState, sol::create, mainState["_G"].tbl };
}

sol::load_result scrinks::lua::load(const std::string& code, const std::string& file)
{
	return mainState.load(code, file);
}

sol::state& scrinks::lua::state()
{
	return mainState;
}



