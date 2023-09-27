#pragma once

#include "effect_pass.h"
#include "pipeline.h"
#include "shader.h"
#include "render_target.h"

namespace scrinks::render::pass
{
	class DeferredLighting : public EffectPass
	{
	public:
		DeferredLighting();
		~DeferredLighting() override;

	public:
		void load_assets() override;
		void setup_draw() override;
	};
}

