#include "lua_bindings.h"
#include <vector>

using namespace scrinks::lua;

std::vector<std::function<void(sol::state&)>> binds{};

bool bindings::register_binding(std::function<void(sol::state&)> func)
{
	binds.push_back(func);
	return true;
}
	
void bindings::setup_all(sol::state& env)
{
	for (const auto& bind : binds)
		bind(env);
}