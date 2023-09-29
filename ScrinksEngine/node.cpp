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

const char* scrinks::core::Node::type() const
{
	return default_name().data();
}

void scrinks::core::Node::fixed_update()
{
	for (auto child : m_children)
	{
		if (child)
			child->fixed_update();
	}
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
