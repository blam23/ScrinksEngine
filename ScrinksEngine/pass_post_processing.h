#pragma once

#include "effect_pass.h"

namespace scrinks::render::pass
{
	class PostProcessing : public EffectPass
	{
	public:
		PostProcessing();
		~PostProcessing() override;

	public:
		void load_assets() override;
		void setup_draw() override;
	};
}

