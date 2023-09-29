#include "lua_engine.h"
#include "window.h"
#include "lua_init_nodes.h"

#include <iostream>

using namespace scrinks::lua;

#define POP_ON_EXIT() AutoPop __pop__{ m_state };

Engine::Engine()
	: m_state{ nullptr }
	, m_paramCount{ 0 }
{
	m_state = luaL_newstate();

	if (!m_state)
		throw std::exception("Unable to initialise Lua Engine.");

	luaL_openlibs(m_state);
}

Engine::~Engine()
{
	if (m_state)
		lua_close(m_state);
}

void fixed_updates()
{
}

Result Engine::init()
{
	Result res{ run_file("lua/init.lua") };

	if (res.m_success)
		scrinks::Window::register_for_fixed_updates(fixed_updates);

	init_nodes();

	return res;
}

void Engine::register_function(const char* name, lua_CFunction func)
{
	lua_pushcfunction(m_state, func);
	lua_setglobal(m_state, name);
}

void scrinks::lua::Engine::register_fixed_update_callback()
{
	
}

Result Engine::run(const char* input)
{
	int result = luaL_loadstring(m_state, input);
	if (result)
	{
		POP_ON_EXIT();
		return lua_tostring(m_state, -1);
	}

	result = lua_pcall(m_state, 0, LUA_MULTRET, 0);
	if (result)
	{
		POP_ON_EXIT();
		return lua_tostring(m_state, -1);
	}

	return true;
}

Result Engine::run_file(const char* file)
{
	int result = luaL_loadfile(m_state, file);
	if (result)
	{
		POP_ON_EXIT();
		return lua_tostring(m_state, -1);
	}

	result = lua_pcall(m_state, 0, LUA_MULTRET, 0);
	if (result)
	{
		POP_ON_EXIT();
		return lua_tostring(m_state, -1);
	}

	return true;
}

bool Engine::setup_call(const char* function)
{
	m_paramCount = 0;
	lua_getglobal(m_state, function);
	return lua_isfunction(m_state, -1);
}

void Engine::setup_call(int reference)
{
	m_paramCount = 0;
	push_reference(reference);
}

int Engine::new_reference_from_stack()
{
	return luaL_ref(m_state, LUA_REGISTRYINDEX);
}

void Engine::delete_reference(int reference)
{
	luaL_unref(m_state, LUA_REGISTRYINDEX, reference);
}

void Engine::push_reference(int reference)
{
	lua_rawgeti(m_state, LUA_REGISTRYINDEX, reference);
}

template<>
std::string Engine::get<std::string>(const char* name) const
{
	POP_ON_EXIT();

	lua_getglobal(m_state, name);

	if (lua_isstring(m_state, -1))
		return lua_tostring(m_state, -1);

	return {};
}

template<>
bool Engine::get<bool>(const char* name) const
{
	POP_ON_EXIT();

	lua_getglobal(m_state, name);

	if (lua_isboolean(m_state, -1))
		return lua_toboolean(m_state, -1);

	return false;
}

template<>
bool Engine::pop()
{
	POP_ON_EXIT();

	if (lua_isboolean(m_state, -1))
		return lua_toboolean(m_state, -1);

	return false;
}

template<>
double Engine::pop()
{
	POP_ON_EXIT();

	if (lua_isnumber(m_state, -1))
		return lua_tonumber(m_state, -1);

	return 0.0;
}

template<>
std::string Engine::pop()
{
	POP_ON_EXIT();

	if (lua_isstring(m_state, -1))
		return lua_tostring(m_state, -1);

	return {};
}

Result Engine::execute_call()
{
	int result = lua_pcall(m_state, m_paramCount, LUA_MULTRET, 0);

	m_paramCount = 0;

	if (result)
	{
		POP_ON_EXIT();
		return lua_tostring(m_state, -1);
	}

	return true;
}

void Engine::abandon_call()
{
	while (m_paramCount > 0)
	{
		lua_pop(m_state, -1);
		m_paramCount--;
	}
}

template<>
void Engine::push(const char* value)
{
	lua_pushstring(m_state, value);
	m_paramCount++;
}

template<>
void Engine::push(std::int32_t value)
{
	lua_pushinteger(m_state, value);
	m_paramCount++;
}

template<>
void Engine::push(std::int64_t value)
{
	lua_pushinteger(m_state, value);
	m_paramCount++;
}

template<>
void Engine::push(std::uint64_t value)
{
	lua_pushinteger(m_state, value);
	m_paramCount++;
}

template<>
void Engine::push(double value)
{
	lua_pushnumber(m_state, value);
	m_paramCount++;
}

AutoPop::~AutoPop()
{
	lua_pop(m_state, 1);
}

AutoReference::~AutoReference()
{
	if (m_engine)
		m_engine->delete_reference(m_reference);
}

Engine Engine::instance{};

bool scrinks::lua::setup()
{
	const auto& result{ Engine::instance.init() };

	if (!result.m_success)
		std::cerr << "Unable to load lua engine:" << std::endl << result.m_message << std::endl;

	return result.m_success;
}
