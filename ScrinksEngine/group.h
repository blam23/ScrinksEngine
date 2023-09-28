#pragma once

#include <vector>
#include "node.h"

namespace scrinks::core
{
	class Group : public Node
	{
	public:

	private:
		void claim(Node* node);
		std::vector<Node*> m_children;
		friend Node;
	};
}

