#include "node_2d.h"
#include "lua_engine.h"

using namespace scrinks::core;

Node2D::Node2D(Node* parent, const Transform2D& position)
	: m_current{ position }
	, m_previous{ position }
	, Node{ parent }
{
}

void scrinks::core::Node2D::setup_script_data()
{
	Node::setup_script_data();
	m_script_env["self"] = this;
}

void Node2D::sync_fixed_update()
{
	m_previous = m_current;
	Node::sync_fixed_update();
}

void Node2D::set_position(const glm::vec2& p)
{
	m_current.pos = p;
}

void Node2D::translate(const glm::vec2& offset)
{
	m_current.pos += offset;
}

void Node2D::rotate(float amount)
{
	m_current.rot += amount;
}

void Node2D::set_rotation(float a)
{
	m_current.rot = a;
}


