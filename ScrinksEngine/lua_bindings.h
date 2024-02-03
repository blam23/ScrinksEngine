#pragma once

#include "lua_engine.h"

namespace scrinks::lua::bindings
{
	void setup_all(sol::state& env);
	bool register_binding(std::function<void(sol::state&)> binds);
}
