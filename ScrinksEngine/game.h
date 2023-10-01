#pragma once

#include "helpers.h"
#include "window.h"
#include "Node.h"

namespace scrinks::core
{
	class Game
	{
	public:
		DISABLE_COPY_AND_MOVE(Game);

	public:
		// Fixed state tick rates
		constexpr static std::uint16_t TickRate = 25;
		constexpr static std::uint16_t SkipRate = 1000 / TickRate;
		constexpr static std::uint16_t MaxFrameSkip = 5;

	public:
		static void init(std::unique_ptr<Node>&& root);
		static void sync_fixed_update();
		static void check_resources();
		static Node* root() { return s_root.get(); }

	private:
		Game() = default;
		static std::unique_ptr<Node> s_root;
	};
}

