#pragma once

#include "node.h"
#include "thread_pool.h"

namespace scrinks::core::nodes
{
	class Root : public Node
	{
	public:
		Root(threads::ID thread) : Node{ thread, nullptr } {}
		virtual ~Root() {}

		void start_game();

	protected:
		virtual const std::string_view default_name() const { return "Root"; }
	};
}
