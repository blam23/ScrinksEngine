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
	bool setup();

	sol::environment create_env();
	sol::load_result load(const std::string& code, const std::string& file);
	sol::state& state();
}

