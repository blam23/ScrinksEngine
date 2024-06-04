#pragma once

#include "script.h"
#include "pipeline.h"
#include "helpers.h"

#include <string>

namespace scrinks
{

	class Project
	{
	public:
		static std::optional<Project> from_file(const std::string_view projectFileName);

		enum class Renderer
		{
			Deferred,
			Sprite
		};

		auto name() const -> const std::string& { return m_name; }
		auto entry_script() const { return m_entryScript; }
		void bind_to_state(sol::state&);
		auto renderer() const { return m_renderer; }

	private:
		Project(const std::string& name, Renderer desiredRenderer, std::shared_ptr<lua::Script> entryScript);

		std::string m_name{};
		Renderer m_renderer{};
		std::shared_ptr<lua::Script> m_entryScript{};
		std::vector<std::string> m_classes{};
	};
}
