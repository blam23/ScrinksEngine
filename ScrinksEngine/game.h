#pragma once

#include "helpers.h"
#include "window.h"
#include "node_root.h"
#include "sprite_renderer.h"

namespace scrinks::core
{
	class Game
	{
	public:
		DISABLE_COPY_AND_MOVE(Game);

	public:
		// Fixed state tick rates
		constexpr static std::uint16_t TickRate = 60;
		constexpr static std::uint16_t SkipRate = 1000 / TickRate;
		constexpr static std::uint16_t MaxFrameSkip = 5;

	public:
		static void init(std::string_view gameScriptPath);
		static void update(float interpolate);
		static void sync_fixed_update();
		static void check_resources();
		static nodes::Root* root() { return s_root.get(); }
		static scrinks::render::SpriteRenderer* sprite_renderer() { return s_spriteRenderer.get(); }
		static void shutdown();
		static std::pair<float, float> mouse_pos();

	private:
		Game() = default;
		static inline std::unique_ptr<nodes::Root> s_root{ nullptr };
		static inline std::shared_ptr<scrinks::render::SpriteRenderer> s_spriteRenderer{ nullptr };
	};
}

