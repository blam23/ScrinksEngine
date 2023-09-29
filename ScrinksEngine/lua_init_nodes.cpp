#include "lua_init_nodes.h"

#include "node_3d.h"

void scrinks::lua::init_nodes()
{
	scrinks::core::Node3D::register_lua_methods();
}
