#include "lua_class.h"

#include "script.h"
#include <format>

using namespace scrinks::lua;

auto Class::from_file(const std::string& path) -> std::expected<Class, std::string>
{
	auto script{ ScriptManager::load_and_store(path, path) };

	if (!script || !script->is_loaded())
		return std::unexpected{ std::format("Unable to load class script file : '{}'", path) };

	auto classEnv{ create_env() };
	auto classRes{ script->run_no_cache(classEnv) };

	if (!classRes.valid())
		return std::unexpected{ std::format("Unable to parse class file : '{}'", script->asset_name()) };

	sol::object classNameObj = classEnv["__class_name"];
	if (classNameObj.get_type() != sol::type::string)
		return std::unexpected{ std::format("Class did not contain '__class_name' metafield string: '{}'", script->asset_name()) };

	const auto name{ classNameObj.as<std::string>() };
	std::string base_node{ "node" };

	sol::object baseNodeObj = classEnv["__base_node"];
	if (baseNodeObj.get_type() == sol::type::string)
		base_node = baseNodeObj.as<std::string>();

	sol::object propsObj = classEnv["__props"];
	if (propsObj.get_type() != sol::type::nil && propsObj.get_type() != sol::type::table)
		return std::unexpected{ std::format("Unknown '__props' field type (should be nil or table): {}.", static_cast<int>(propsObj.get_type())) };

	return Class{ std::move(classEnv), std::move(classRes) };
}

auto Class::try_run_function(const std::string& func) -> std::expected<sol::function_result, std::string>
{
	if (m_env.valid())
	{
		sol::function luaFunc = m_env[func];
		if (luaFunc.valid())
		{
			sol::set_environment(m_env, luaFunc);
			sol::function_result res{ luaFunc() };
			if (!res.valid())
			{
				sol::error msg = res;
				return std::unexpected{ std::format("Error calling func <{}>, error: {}", func, msg.what()) };
			}

			return res;
		}

		return std::unexpected{ std::format("Failed to call <{}>, not a valid function.", func) };
	}

	return std::unexpected{ std::format("Failed to call <{}>, invalid environment.", func) };
}

Class::Class(sol::environment&& env, sol::function_result&& res)
	: m_env{ std::move(env) }, m_res{ std::move(res) }
{
}

