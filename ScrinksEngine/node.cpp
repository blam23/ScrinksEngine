#include "node.h"
#include "group.h"

#include <memory>
#include <typeinfo>
#include "metadata_helpers.h"

using namespace scrinks::core;

std::atomic<Node::ID> Node::s_id{ 0 };

Node::Node(Node* m_parent)
	: m_parent{ m_parent }
	, m_name{}
	, m_script{ nullptr }
	, m_id { s_id++ }
	, m_script_env { lua::create_env() }
{
	if (m_parent)
		m_parent->claim_child(*this);
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
	sol::function fixedUpdate = m_script_env[func];
	if (fixedUpdate.valid())
	{
		sol::set_environment(m_script_env, fixedUpdate);
		sol::function_result res{ fixedUpdate() };
		if (!res.valid())
		{
			sol::error msg = res;
			std::cerr << "Error calling func <" << func << ">, error: " << msg.what() << std::endl;
		}
	}
}

void Node::reload_script_if_outdated()
{
	if (m_script && m_script->is_outdated())
	{
		auto newScript{ scrinks::lua::ScriptManager::instance().get(m_script->asset_name()) };

		if (newScript)
			set_script(newScript);
	}
}

void Node::fixed_update()
{
	reload_script_if_outdated();

	run_func_checked("fixed_update");

	for (Node* child : m_children)
	{
		if (child)
			child->fixed_update();
	}
}

void Node::set_script(std::shared_ptr<lua::Script> script)
{
	m_script = script;

	if (m_script)
	{
		auto res = m_script->run(m_script_env);
		if (!res.valid())
		{
			sol::error err = res;
			std::cerr << "Failed to set script: " << err.what() << std::endl;
		}

		setup_script_data();
		run_func_checked("script_added");
	}
}

void Node::setup_script_data()
{
	m_script_env["name"]
		= [this] () { return name(); };
}

void Node::claim_child(Node& node)
{
	if (node.m_parent)
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
