#include "group.h"

using namespace scrinks::core;

void scrinks::core::Group::claim(Node* node)
{
	m_children.push_back(node);
}
