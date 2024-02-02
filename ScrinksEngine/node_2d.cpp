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
	m_script_env["set_pos"]
		= [this] (float x, float y) { return set_position(x, y); };

	m_script_env["translate"]
		= [this] (float x, float y) { return translate(x, y); };

	m_script_env["rotate"]
		= [this] (float a) { return rotate(a); };

	m_script_env["set_rotation"]
		= [this] (float a) { return set_rotation(a); };

	Node::setup_script_data();
}

void Node2D::sync_fixed_update()
{
	m_previous = m_current;
	Node::sync_fixed_update();
}

void Node2D::set_position(const glm::vec2& pos)
{
	m_current.pos = pos;
}

void Node2D::set_position(float x, float y)
{
	m_current.pos = glm::vec2(x, y);
}

void Node2D::translate(const glm::vec2& offset)
{
	m_current.pos += offset;
}

void Node2D::translate(float x, float y)
{
	m_current.pos += glm::vec2(x, y);
}

void Node2D::rotate(float amount)
{
	m_current.rot += amount;
}

void Node2D::set_rotation(float a)
{
	m_current.rot = a;
}


