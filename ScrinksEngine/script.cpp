#include "script.h"
#include "lua_engine.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace scrinks::lua;

Script::Script(Badge<ScriptManager>, const std::string& code)
	: Asset{ 0 }
{
	Engine::instance.run(code.c_str());
}

template <>
void Script::call(core::Node*)
{

}

template <>
std::shared_ptr<Script> ScriptManager::load(const ScriptDescription& description)
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
		return std::make_shared<Script>(Badge<ScriptManager>{}, code);
	}
	catch (std::ifstream::failure error)
	{
		std::cerr << "Error loading script <" << description << ">: '" << error.what() << "'." << std::endl;
	}

	return nullptr;
}
