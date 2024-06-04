#pragma once

#include "lua_engine.h"

#include <string>
#include <expected>

namespace scrinks::lua
{
	class Class
	{
	public:
		static auto from_file(const std::string& file) -> std::expected<Class, std::string>;
		auto try_run_function(const std::string& name) -> std::expected<sol::function_result, std::string>;

	private:
		Class(sol::environment&& env, sol::function_result&& res);

		sol::environment m_env;
		sol::function_result m_res;
	};
}
