#include "project.h"
#include "script.h"
#include "lua_engine.h"

using namespace scrinks;

#define TRUE_OR_FAIL(check, msg) if(!(check)) { std::cout << msg << std::endl; return {}; }

std::optional<Project> Project::from_file(const std::string_view projectFileName)
{
	auto projectScript = lua::ScriptManager::load_and_store(projectFileName.data(), projectFileName.data());

	// TODO: fix this whole mess
	lua::ScriptManager::mark_for_removal(projectFileName.data());
	
	TRUE_OR_FAIL(projectScript && projectScript->is_loaded(), "Unable to load project file: " << projectFileName);

	auto env{ lua::create_env() };
	auto res{ projectScript->run(env) };
	TRUE_OR_FAIL(res.valid(), "Unable to parse project file: " << projectFileName);

	std::string name   = env["name"];
	std::string render = env["render"];
	std::string entry  = env["entry"];

	TRUE_OR_FAIL(!name.empty(), "Project description did not contain 'name' field.");
	TRUE_OR_FAIL(!entry.empty(), "Project description did not contain 'entry' field pointing to entry script file.");

	auto entryScript = lua::ScriptManager::load_and_store(entry, entry);
	TRUE_OR_FAIL(entryScript && entryScript->is_loaded(), "Unable to load entry script file: " << entry);

	sol::table classes = env["classes"];

	// only do basic validation here.
	for (const auto& script : classes)
	{
		TRUE_OR_FAIL(script.second.get_type() == sol::type::string, "Unable to register class, unknown type (expected path as string).");
		
		auto path = script.second.as<std::string>();
		TRUE_OR_FAIL(lua::register_class(path), "Failed to register class: " << path);
	}

	return Project {
		name,
		render.empty() ? "deferred" : render,
		entryScript
	};
}

void Project::bind_to_state(sol::state& /*state*/)
{

}

Project::Project(
	const std::string& name,
	const std::string& renderEngine,
	std::shared_ptr<lua::Script> entryScript
)
	: m_name{ name }
	, m_renderEngine{ renderEngine }
	, m_entryScript{ entryScript }
{
}
