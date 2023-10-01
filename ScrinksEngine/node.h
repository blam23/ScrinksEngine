#pragma once

#include "helpers.h"
#include "script.h"
#include "lua_engine.h"
#include "thread_pool.h"
#include <vector>
#include <cstdint>
#include <string>
#include <string_view>
#include <atomic>
#include <map>
#include <memory>

namespace scrinks::core
{
	class Node
	{
	public:
		using ID = std::size_t;
		constexpr static ID InvalidID{ (std::numeric_limits<ID>::max)() };

		Node(Node* m_parent, threads::Group threadGroup = threads::Group::Main);
		virtual ~Node();

	public:
		std::vector<Node*>& children() { return m_children; }
		const std::vector<Node*>& children() const { return m_children; }
		const char* name() const;
		const char* type() const;
		ID id() const { return m_id; }
		void rename(const std::string& name) { m_name = name; }

		virtual void fixed_update();    // called at a fixed rate (per Game::TickRate)
		virtual void check_resources(); // called when resources <may> have been updated.
		virtual void attached() {}      // (just) attached to a parent node
		virtual void removed() {}       // (about to be) removed from parent node

		void set_script(std::shared_ptr<lua::Script> script);

		// If you're gonna add a shit tonne of child nodes..
		void reserve_child_nodes(size_t amount);

	protected:
		virtual const std::string_view default_name() const { return "Node"; }
		void run_func_checked(const std::string& func);
		virtual void setup_script_data();

	private:
		void claim_child(Node& node);
		void disown_child(Node& node);
		void reload_script_if_outdated();
		void setup_env_for_thread();

	protected:
		sol::environment m_script_env;
		threads::Reference m_thread;

	private:
		Node* m_parent;
		std::vector<Node*> m_children;
		std::shared_ptr<scrinks::lua::Script> m_script;
		std::string m_name;
		ID m_id;

		static std::atomic<ID> s_id;

	};
};
