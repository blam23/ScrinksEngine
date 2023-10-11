#include "script.h"
#include "lua_engine.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace scrinks::lua;

Script::Script(Badge<ScriptManager>, const std::string& name, const std::string& code)
	: Asset{ name, 0 }
	, m_strCode{ code }
{
	sol::load_result res{ lua::load(code, name) };

	if (res.valid())
	{
		m_code = res.get<sol::function>();
		m_loaded = true;
	}
	else
	{
		sol::error err = res;
		std::cerr << "Failed to load script: " << err.what() << std::endl;
	}
}

sol::function_result Script::run(sol::environment& env)
{
	sol::set_environment(env, m_code);
	return m_code();
}

sol::function_result Script::run_no_cache(sol::environment& env)
{
	sol::function res{ lua::load(m_strCode, m_name) };

	if (res.valid())
	{
		sol::set_environment(env, res);
		return res();
	}

	return {};
}

template <>
std::shared_ptr<Script> ScriptManager::load(const std::string& name, const ScriptDescription& description)
{
	std::string code;
	std::ifstream file;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		file.open(description);

		std::stringstream stream;
		stream << file.rdbuf();

		file.close();

		code = stream.str();
		return std::make_shared<Script>(Badge<ScriptManager>{}, name, code);
	}
	catch (const std::ifstream::failure& error)
	{
		std::cerr << "Error loading script <" << description << ">: '" << error.what() << "'." << std::endl;
	}

	return nullptr;
}
