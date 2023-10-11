#include "node_3d.h"
#include "lua_engine.h"

using namespace scrinks::core;

Node3D::Node3D(Node* parent, const Transform& position)
	: m_current{ position }
	, m_previous{ position }
	, Node { parent }
{
}

void scrinks::core::Node3D::setup_script_data()
{
	m_script_env["set_pos"]
		= [this] (float x, float y, float z) { return set_position(x, y, z); };

	m_script_env["translate"]
		= [this] (float x, float y, float z) { return translate(x, y, z); };

	m_script_env["rotate"]
		= [this] (float a, float x, float y, float z) { return rotate(a, glm::vec3{ x, y, z }); };

	m_script_env["set_rotation"]
		= [this] (float x, float y, float z) { return set_rotation(glm::vec3{ x, y, z }); };

	m_script_env["set_scale"]
		= [this] (float x, float y, float z) { return set_scale(x, y, z); };

	Node::setup_script_data();
}

void Node3D::sync_fixed_update()
{
	m_previous = m_current;
	Node::sync_fixed_update();
}

void Node3D::set_position(const glm::vec3& pos)
{
	m_current.pos = pos;
}

void Node3D::set_position(float x, float y, float z)
{
	m_current.pos = glm::vec3(x, y, z);
}

void Node3D::translate(const glm::vec3& offset)
{
	m_current.pos += offset;
}

void Node3D::translate(float x, float y, float z)
{
	m_current.pos += glm::vec3(x, y, z);
}

void Node3D::rotate(float amount, const glm::vec3& axis)
{
	m_current.rot = glm::rotate(m_current.rot, amount, axis);
}

void Node3D::set_scale(float amount)
{
	m_current.scale = glm::vec3(amount);
}

void Node3D::set_scale(float x, float y, float z)
{
	m_current.scale = glm::vec3(x, y, z);
}


void Node3D::set_rotation(glm::quat& q)
{
	m_current.rot = q;
}

void Node3D::set_rotation(const glm::vec3& rot)
{
	m_current.rot = glm::quatLookAt(rot, glm::vec3(0.0f, 1.0f, 0.0f));
}


