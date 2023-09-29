#include "node_3d.h"
#include "lua_engine.h"

using namespace scrinks::core;

static scrinks::lua::Engine& engine{ scrinks::lua::Engine::instance };

scrinks::core::Node3D::Node3D(Node* parent, const Transform& position)
	: m_current{ position }
	, m_previous{ position }
	, Node { parent }
{
}

namespace binds
{
	int set_pos(lua_State* state)
	{
		lua_checkstack(state, 3);
		lua_isnumber(state, 1);
		lua_isnumber(state, 2);
		lua_isnumber(state, 3);

		float x{ (float)lua_tonumber(state, 1) };
		float y{ (float)lua_tonumber(state, 2) };
		float z{ (float)lua_tonumber(state, 3) };

		Node3D* caller = (Node3D*)engine.calling_node();
		caller->set_position(x, y, z);
		
		return 0;
	}
}

void _bind(lua_State* L, const std::string& name, lua_CFunction func)
{
	lua_pushstring(L, name.c_str());
	lua_pushcfunction(L, func);
	lua_settable(L, -3);
}

void scrinks::core::Node3D::register_lua_methods()
{
	lua_State* L{ engine.m_state };

	lua_newtable(L);
	_bind(L, "set_pos", &binds::set_pos);
	lua_setglobal(L, "Node3D");
}

void scrinks::core::Node3D::fixed_update()
{
	m_previous = m_current;
	Node::fixed_update();
}

void Node3D::set_position(const glm::vec3& pos)
{
	m_current.pos = pos;
}

void Node3D::set_position(float x, float y, float z)
{
	m_current.pos = glm::vec3(x, y, z);
}

void scrinks::core::Node3D::translate(const glm::vec3& offset)
{
	m_current.pos += offset;
}

void scrinks::core::Node3D::translate(float x, float y, float z)
{
	m_current.pos += glm::vec3(x, y, z);
}

void scrinks::core::Node3D::rotate(float amount, const glm::vec3& axis)
{
	m_current.rot = glm::rotate(m_current.rot, amount, axis);
}

void scrinks::core::Node3D::set_scale(float amount)
{
	m_current.scale = glm::vec3(amount);
}

void scrinks::core::Node3D::set_rotation(glm::quat& q)
{
	m_current.rot = q;
}
