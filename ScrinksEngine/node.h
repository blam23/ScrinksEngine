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

		DISABLE_COPY_AND_MOVE(Node);

		using ID = std::size_t;
		constexpr static ID InvalidID{ (std::numeric_limits<ID>::max)() };

		Node(Node* m_parent, threads::Group threadGroup = threads::Group::Split);
		virtual ~Node();

	public:
		std::vector<Node*>& children() { return m_children; }
		const std::vector<Node*>& children() const { return m_children; }
		Node* parent() const;
		const char* name() const;
		const char* type() const;
		ID id() const { return m_id; }
		void rename(const std::string& name) { m_name = name; }
		void mark_for_deletion();
		void mark_for_child_cleanup();
		void add_to_group(const std::string& group);
		bool in_group(const std::string& group) const;
		
		void set_script(std::shared_ptr<lua::Script> script);
		void set_and_load_script(std::shared_ptr<lua::Script> script);

		void set_property(const std::string& name, const sol::object& value);
		sol::object get_property(const std::string& name);
		
		// If you're gonna add a shit tonne of child nodes..
		void reserve_child_nodes(size_t amount);
		scrinks::threads::ID thread_id() const { return m_thread.m_thread_id; }

		template <
			typename T_Node,
			class... T_Args,
			typename = std::enable_if_t<std::is_base_of<Node, T_Node>::value>
		>
		T_Node* new_child(T_Args&&... args)
		{
			return new T_Node(this, std::forward<T_Args>(args)...);
		}

		template <typename T>
		std::vector<T*> get_children_of_type() {
			std::vector<T*> result;
			for (auto& child : m_children) {
				if (dynamic_cast<T*>(child) != nullptr) {
					result.push_back(static_cast<T*>(child));
				}
			}
			return result;
		}
		//
		// EVENTS
		//
		
		//
		// NOT RECURSIVE
		virtual void fixed_update();    // called at a fixed rate (per Game::TickRate)
		virtual void child_added(Node&) {} // (just) became a child node
		virtual void child_removed(Node&) {} // (about to be) removed as a child node
		virtual void attached() {}      // (just) attached to a parent node
		virtual void removed() {}       // (about to be) removed from parent node

		//
		// RECURSIVE - updates children
		virtual void check_resources(); // called when resources <may> have been updated.
		virtual void sync_fixed_update();
		//
		//
		//

	public:
		static void dispatch_fixed_update(void* node) { ((Node*)node)->fixed_update(); }

		// TODO: This whole group system is terrible, fix.
		static void add_group(const std::string& name);
		static std::vector<Node*> get_nodes_in_group(const std::string& name);

		template <typename T_NODE>
		static const std::vector<T_NODE*> get_typed_nodes_in_group(const std::string& name)
		{
			std::lock_guard lock{ s_group_mutex };

			//todo: ewwww
			std::vector<T_NODE*> ret{};

			for (const auto node : get_nodes_in_group(name))
			{
				if (const auto n = dynamic_cast<T_NODE*>(node); n)
				{
					ret.push_back(n);
				}
			}

			return ret;
		}

	protected:
		virtual const std::string_view default_name() const { return "Node"; }
		void run_func_checked(const std::string& func);
		virtual void setup_script_data();
		void load_script();

	private:
		void cleanup_children();
		void claim_child(Node& node);
		void disown_child(Node& node);
		void reload_script_if_outdated();
		bool check_has_field(const std::string& name);
		bool on_correct_thread();
		void copy_shared_data();

	protected:
		sol::environment m_script_env;
		threads::Reference m_thread;

	private:
		bool m_marked_for_deletion;
		bool m_requires_cleanup;

		std::mutex m_property_mutex;
		std::mutex m_child_mutex;
		std::vector<Node*> m_children;
		Node* m_parent;
		std::shared_ptr<lua::Script> m_script;
		std::string m_name;
		std::uint16_t m_group{ 0 };
		ID m_id;
		bool m_init_lua;
		bool m_has_fixed_update{ false };
		bool m_has_sync_fixed_update{ false };
		std::map<std::string, sol::object> m_data;
		std::map<std::string, lua::SharedID> m_shared;

		static std::atomic<ID> s_id;

		static inline std::mutex s_group_mutex{};
		static inline std::vector<std::vector<Node*>> s_groups{ {} };
		static inline std::map<std::string, std::uint16_t> s_group_names{};
	};
};
