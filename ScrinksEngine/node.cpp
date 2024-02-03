#include "node.h"
#include "group.h"
#include "game.h"

#include <memory>
#include <typeinfo>
#include <thread>
#include "metadata_helpers.h"

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

void scrinks::core::Node::reserve_child_nodes(size_t amount)
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

bool scrinks::core::Node::check_has_field(const std::string& name)
{
	if (m_script && m_script_env.valid())
	{
		sol::function luaFunc = m_script_env[name];
		return luaFunc.valid();
	}

	return false;
}

void Node::sync_fixed_update()
{
	if (m_has_sync_fixed_update)
		run_func_checked("sync_fixed_update");

	for (Node* child : m_children)
	{
		if (child)
			child->sync_fixed_update();
	}
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

	m_data = m_script_env.create();
	setup_script_data();
	run_func_checked("script_added");

	m_has_fixed_update = check_has_field("fixed_update");
	m_has_sync_fixed_update = check_has_field("sync_fixed_update");
	m_init_lua = true;
}

void Node::fixed_update()
{
	if (!m_init_lua && m_script)
		load_script();

	if (m_has_fixed_update)
		run_func_checked("fixed_update");
}

void scrinks::core::Node::check_resources()
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

void Node::setup_script_data()
{
	m_script_env["this_node"] = this;

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
