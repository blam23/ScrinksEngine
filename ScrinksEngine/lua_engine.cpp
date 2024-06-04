#include "lua_engine.h"
#include "lua_bindings.h"
#include "lua_class.h"
#include "window.h"
#include "game.h"
#include "node.h"

#include <iostream>
#include <mutex>
#include <list>
#include "spdlog/spdlog.h"

using namespace scrinks::lua;

thread_local sol::state mainState;
thread_local bool attemptedSetup;

std::list<std::string> registeredClasses{};

scrinks::core::Node* currentNode{ nullptr };

static bool load_class(const std::string& path)
{
	spdlog::info("loading {}..", path);

	auto result{ Class::from_file(path) };
	const auto ret{ result.has_value() };

	if (ret)
	{
		auto& c{ result.value() };
		mainState["_G"][c.name()] = c.env();
	}
	else
	{
		spdlog::warn("Failed to load class from file: '{}', error: {}", path, result.error());
	}

	return ret;
}

std::mutex load_mutex{};
auto scrinks::lua::load_classes() -> bool
{
	std::lock_guard lock{ load_mutex };
	spdlog::info("loading classes..");
	for (const auto& path : registeredClasses)
	{
		bool loaded{ load_class(path) };
		if (!loaded)
			return false;
	}

	return true;
}

void scrinks::lua::setup()
{
	mainState.open_libraries
	(
		sol::lib::base,
		sol::lib::package,
		sol::lib::math,
		sol::lib::string,
		sol::lib::table,
		sol::lib::bit32
	);

	const auto res{ mainState.do_file("lua/init.lua") };
	if (!res.valid())
	{
		sol::error err = res;
		spdlog::error("Failed to load init script", err.what());
	}
	else
	{
		spdlog::info("Loaded lua!");
	}

	bindings::setup_all(mainState);

	attemptedSetup = true;
}

sol::environment scrinks::lua::create_env()
{
	if (!attemptedSetup)
		setup();

	return { mainState, sol::create, mainState.globals() };
}

sol::load_result scrinks::lua::load(const std::string& code, const std::string& file)
{
	return mainState.load(code, file);
}

void scrinks::lua::dbg(sol::environment& env, const std::string& code)
{
	mainState.script(code, env);
}

void scrinks::lua::dbg_print_globals(sol::environment& env)
{
	mainState.script("for k,v in pairs(_G) do print(k,v) end", env);
}

sol::object scrinks::lua::copy_object(const sol::object& in, sol::state& to)
{
	auto type = in.get_type();

	switch (type)
	{
		case sol::type::string:   return sol::make_object(to, in.as<std::string>());
		case sol::type::number:   return sol::make_object(to, in.as<double>());
		case sol::type::boolean:  return sol::make_object(to, in.as<bool>());

		case sol::type::userdata:
		{
			if (in.is<glm::vec2>()) {
				const auto& in_vec{ in.as<glm::vec2>() };
				return sol::make_object<glm::vec2>(to, in_vec.x, in_vec.y);
			}
			return {};
		}

		case sol::type::table:
		{
			auto orig = in.as<sol::table>();
			auto clone = to.create_table();

			for (const auto& [k, v] : orig)
				clone.set(copy_object(k, to), copy_object(v, to));

			return clone;
		}

		// todo?
		case sol::type::function:
		case sol::type::lightuserdata:

		// ignore
		case sol::type::thread:
		case sol::type::none:
		case sol::type::lua_nil:
		case sol::type::poly:
		default:
			return {};
	}
}

auto scrinks::lua::register_class(const std::string& path) -> void
{
	registeredClasses.push_back(path);
}

//
// Shared State
// Used to ferry data between different lua states.
//

std::map<SharedID, sol::object> s_sharedState{};
SharedID s_lastID;
sol::state sharedState;
bool setupSharedState{ false };
std::mutex shareMutex;

void check_shared_state()
{
	if (!setupSharedState)
	{
		sharedState.open_libraries();
		setupSharedState = true;
	}
}

SharedID scrinks::lua::send_to_shared(const sol::object& obj)
{
	std::lock_guard lock{ shareMutex };

	check_shared_state();

	SharedID id{ s_lastID };
	s_sharedState[id] = copy_object(obj, sharedState);

	s_lastID++;
	if (s_lastID == (std::numeric_limits<size_t>().max)())
		s_lastID = 0;

	return id;
}

sol::object scrinks::lua::get_from_shared(SharedID id)
{
	std::lock_guard lock{ shareMutex };

	if (!setupSharedState)
		return sol::nil;

	if (const auto itr = s_sharedState.find(id); itr != s_sharedState.end())
	{
		auto ret{ copy_object(itr->second, mainState) };
		s_sharedState.erase(itr);

		return ret;
	}

	return sol::nil;
}

