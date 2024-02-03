#pragma once

#include "node.h"

namespace scrinks::core::nodes
{
	class Root : public Node
	{
	public:
		Root() : Node{ nullptr } {}
		virtual ~Root() {}

		void start_game();

	protected:
		virtual const std::string_view default_name() const { return "Root"; }
	};
}
