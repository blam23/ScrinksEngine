#pragma once

#include "helpers.h"
#include "node.h"
#include <string>
#include <functional>
#include <optional>

#include <luajit/lua.hpp>
#include <luajit/lualib.h>
#include <luajit/lauxlib.h>

namespace scrinks::lua
{
	bool setup();

	struct Result
	{
		bool m_success;
		std::string m_message;

		operator bool() const { return m_success; }

		Result(const char* error) : m_success{ false }, m_message{ error }
		{
		}

		Result(bool success) : m_success{ success }, m_message{}
		{
		}
	};

	// AUTO POPS, ROLL OUT
	class AutoPop
	{
	public:
		DISABLE_COPY_AND_MOVE(AutoPop);

	public:
		AutoPop(lua_State* state) : m_state{ state } { }
		~AutoPop();

	private:
		lua_State* m_state;
	};

	class Engine
	{
	public:
		DISABLE_COPY_AND_MOVE(Engine);

	public:
		Engine();
		~Engine();

		static Engine instance;

	public:
		Result init();

		void set_calling_node(core::Node* node) { m_currentNode = node; }
		core::Node* calling_node() const { return m_currentNode; }

		void register_function(const char* name, lua_CFunction func);
		void register_fixed_update_callback();

		Result run(const char* input);
		Result run_file(const char* file);

		bool setup_call(const char* functionName);
		void setup_call(int reference);

		// Keeps alive the lua object on top of the stack. (table, function, etc.)
		// This object can be referenced in C++ via the return value.
		// Make sure to call delete_reference to let the object be GC'd. (Or use AutoReference)
		int  new_reference_from_stack();
		void delete_reference(int reference);
		void push_reference(int reference);

		Result execute_call();
		void abandon_call();

		template <class T> void push(T value);
		template <class T> T get(const char* name) const;
		template <class T> T pop();

	public:
		lua_State* m_state;
	
	private:
		std::uint32_t m_paramCount;
		core::Node* m_currentNode;
	};

	struct AutoReference
	{
		AutoReference(Engine* engine, int reference)
			: m_engine{ engine },
			m_reference{ reference }
		{}

		~AutoReference();

		operator int() const { return m_reference; }

		int m_reference;

	private:
		Engine* m_engine;
	};
}

