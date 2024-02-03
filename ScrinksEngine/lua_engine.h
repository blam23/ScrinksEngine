#pragma once

#include "helpers.h"
#include <string>
#include <functional>
#include <optional>
#include <memory>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace scrinks::lua
{
	void setup();

	sol::environment create_env();
	sol::load_result load(const std::string& code, const std::string& file);
	void dbg(sol::environment& env, const std::string& code);
	void dbg_print_globals(sol::environment& env);
	sol::state& state();
	sol::object copy_object(const sol::object& in, sol::environment& to);
}

