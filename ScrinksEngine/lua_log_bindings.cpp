#include "lua_bindings.h"
#include "script.h"
#include "node.h"
#include "spdlog/spdlog.h"

using namespace scrinks;

static void setup_log_bindings(sol::state& env)
{
	auto log_table = env.create_table();

	log_table["info"] = [] (const std::string& message)
	{
		spdlog::info(message);
	};
	log_table["warn"] = [] (const std::string& message)
	{
		spdlog::warn(message);
	};
	log_table["error"] = [] (const std::string& message)
	{
		spdlog::error(message);
	};

	env["log"] = log_table;
}

static bool registered = lua::bindings::register_binding(&setup_log_bindings);
