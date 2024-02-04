#include "node.h"
#include "group.h"
#include "game.h"

#include <memory>
#include <typeinfo>
#include <thread>
#include "metadata_helpers.h"
#include <glm/gtx/string_cast.hpp>

using namespace scrinks::core;

std::atomic<Node::ID> Node::s_id{ 0 };

Node::Node(Node* parent, threads::Group threadGroup)
	: m_name{}
	, m_script{ nullptr }
	, m_id { s_id++ }
	, m_thread{ threads::Reference{ this, threadGroup } }
	, m_parent{ nullptr }
{
	if (parent)
		parent->claim_child(*this);
}

Node::~Node()
{
	for (Node* node : m_children)
	{
		if (node)
			delete node;
	}
}

const char* Node::name() const
{
	return m_name.empty() ? default_name().data() : m_name.c_str();
}

const char* Node::type() const
{
	return default_name().data();
}

void Node::reserve_child_nodes(size_t amount)
{
	m_children.reserve(amount);
}

void Node::run_func_checked(const std::string& func)
{
	if (m_script && m_script_env.valid())
	{
		sol::function luaFunc = m_script_env[func];
		if (luaFunc.valid())
		{
			sol::set_environment(m_script_env, luaFunc);
			sol::function_result res{ luaFunc() };
			if (!res.valid())
			{
				sol::error msg = res;
					std::cerr << "Error calling func <" << func << ">, error: " << msg.what() << std::endl;
			}
		}
	}
}

void Node::reload_script_if_outdated()
{
	if (m_script && m_script->is_outdated())
	{
		auto newScript{ scrinks::lua::ScriptManager::get(m_script->asset_name()) };

		if (newScript)
			set_script(newScript);
	}
}

bool Node::check_has_field(const std::string& name)
{
	if (m_script && m_script_env.valid())
	{
		sol::function luaFunc = m_script_env[name];
		return luaFunc.valid();
	}

	return false;
}

void Node::cleanup_children()
{
	size_t i = m_children.size() - 1;

	while (true)
	{
		const auto child = m_children[i];

		if (!child)
		{
			m_children.erase(m_children.begin() + i);
		}
		else if (child->m_marked_for_deletion)
		{
			m_children.erase(m_children.begin() + i);
			delete child;
		}
		else
		{
			if (child->m_requires_cleanup)
				child->cleanup_children();
		}

		if (i == 0)
			break;

		i--;
	}
}

bool Node::on_correct_thread()
{
	return threads::on_my_thread(m_thread);
}

void Node::copy_shared_data()
{
	if (!on_correct_thread())
		return;

	for (const auto& [key, value] : m_shared)
		m_data[key] = lua::get_from_shared(value);

	m_shared.clear();
}

void Node::sync_fixed_update()
{
	if (m_marked_for_deletion)
		return;

	if (m_requires_cleanup)
		cleanup_children();

	if (m_has_sync_fixed_update)
		run_func_checked("sync_fixed_update");

	for (Node* child : m_children)
	{
		if (child)
			child->sync_fixed_update();
	}
}

void scrinks::core::Node::mark_for_child_cleanup()
{
	m_requires_cleanup = true;

	if (m_parent)
		m_parent->mark_for_child_cleanup();
}


void scrinks::core::Node::mark_for_deletion()
{
	m_marked_for_deletion = true;

	if (m_parent)
		m_parent->mark_for_child_cleanup();
}

void Node::load_script()
{
	m_script_env = lua::create_env();
	auto res = m_script->run_no_cache(m_script_env);
	if (!res.valid())
	{
		sol::error err = res;
		std::cerr << "Failed to set script: " << err.what() << std::endl;
	}

	setup_script_data();
	copy_shared_data();
	run_func_checked("script_added");

	m_has_fixed_update = check_has_field("fixed_update");
	m_has_sync_fixed_update = check_has_field("sync_fixed_update");
	m_init_lua = true;
}

void Node::fixed_update()
{
	if (m_marked_for_deletion)
		return;

	if (!m_init_lua && m_script)
		load_script();

	if (m_has_fixed_update)
	{
		copy_shared_data();
		run_func_checked("fixed_update");
	}
}

void Node::check_resources()
{
	reload_script_if_outdated();

	for (Node* child : m_children)
	{
		if (child)
			child->check_resources();
	}
}

void Node::set_script(std::shared_ptr<lua::Script> script)
{
	m_init_lua = false;
	m_script = script;
}

void Node::set_and_load_script(std::shared_ptr<lua::Script> script)
{
	set_script(script);
	load_script();
}

void Node::set_property(const std::string& name, const sol::object& value)
{
	//const auto id{ lua::send_to_shared(value) };
	//m_shared[name] = id;

	if (on_correct_thread())
	{
		m_data[name] = value;
	}
	else
	{
		// need to copy to shared data and pull from it later when on this node's thread.
		const auto id{ lua::send_to_shared(value) };
		m_shared[name] = id;
	}
}

sol::object Node::get_property(const std::string& name)
{
	if (auto itr = m_data.find(name); itr != m_data.end())
		return itr->second;

	return sol::nil;
}

void Node::setup_script_data()
{
	m_script_env["node"] = this;

	m_script_env["name"]
		= [this] () { return name(); };
}

void Node::claim_child(Node& node)
{
	if (node.m_parent != nullptr)
		node.m_parent->disown_child(node);

	m_children.push_back(&node);
	node.m_parent = this;
	node.attached();
}

void Node::disown_child(Node& node)
{
	for (auto it = m_children.begin(); it != m_children.end(); it++)
	{
		if (*it == &node)
		{
			(*it)->removed();
			m_children.erase(it);
			break;
		}
	}
}
