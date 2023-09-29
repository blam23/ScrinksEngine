#pragma once

#include "helpers.h"
#include "asset.h"

namespace scrinks::lua
{
	class Script;

	using ScriptDescription = std::string;
	using ScriptManager = core::AssetManager<ScriptDescription, Script>;

	class Script : public core::Asset
	{
	public:
		Script(Badge<ScriptManager>, const std::string& code);

	public:
		template <typename T>
		void call(T* caller);
	};
}

