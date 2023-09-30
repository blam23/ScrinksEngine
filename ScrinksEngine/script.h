#pragma once

#include "helpers.h"
#include "lua_engine.h"
#include "asset.h"
#include <map>

namespace scrinks::lua
{
	class Script;

	using ScriptDescription = std::string;
	using ScriptManager = core::AssetManager<ScriptDescription, Script>;

	class Script : public core::Asset
	{
	public:
		Script(Badge<ScriptManager>, const std::string& file, const std::string& code);

	public:
		sol::function_result run(sol::environment& env);

	private:
		sol::function m_code;
	};
}
