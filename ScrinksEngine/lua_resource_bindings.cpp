#include "lua_bindings.h"
#include "script.h"
#include "node.h"

using namespace scrinks;

static void setup_resource_bindings(sol::state& env)
{
	env["load_script"]
		= [] (const std::string& name) { return lua::ScriptManager::load_and_store(name, name); };
}

static bool registered = lua::bindings::register_binding(&setup_resource_bindings);
