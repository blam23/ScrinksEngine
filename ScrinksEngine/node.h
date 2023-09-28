#pragma once

#include "helpers.h"
#include <vector>
#include <cstdint>

namespace scrinks::core
{
	class Node
	{
	public:
		DISABLE_COPY_AND_MOVE(Node);
		using ID = std::uint32_t;

		Node(Node& m_parent);
		virtual ~Node();

	private:
		void claim_child(Node& node);
		void disown_child(Node& node);

	private:
		Node* m_parent;
		std::vector<Node*> m_children;
		//Script* m_script;
	};
};
