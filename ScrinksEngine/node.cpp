#include "node.h"
#include "group.h"

#include <memory>

using namespace scrinks::core;

Node::Node(Node& m_parent)
{
	m_parent.claim_child(*this);
}

Node::~Node()
{
	for (Node* node : m_children)
	{
		if (node)
			node->~Node();
	}
}

void Node::claim_child(Node& node)
{
	if (node.m_parent)
		node.m_parent->disown_child(node);

	m_children.push_back(&node);
	node.m_parent = this;
}

void Node::disown_child(Node& node)
{
	for (auto it = m_children.begin(); it != m_children.end(); it++)
	{
		if (*it == &node)
		{
			m_children.erase(it);
			break;
		}
	}
}
