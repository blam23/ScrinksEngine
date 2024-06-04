#pragma once

#include "lua_engine.h"
#include "text_file_asset.h"

#include <string>
#include <expected>

namespace scrinks::lua
{
	class Class
	{
	public:
		static auto from_file(const std::string& file) -> std::expected<Class, std::string>;
		auto try_run_function(const std::string& name) -> std::expected<sol::function_result, std::string>;

		auto& name() const { return m_name; };
		auto& env() const { return m_env; };

	private:
		Class(std::string&& name, sol::environment&& env);

		sol::environment m_env;
		std::string m_name;

		static inline std::shared_ptr<core::TextFile> s_setup_template{};
	};
}
