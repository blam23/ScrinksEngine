#pragma once

#include "effect_pass.h"
#include "pass_shadow_map.h"
#include "pass_environment_map.h"

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
		DeferredLighting* set_shadow_map(ShadowMap* map) { m_map = map; return this; }
		DeferredLighting* set_env_map(EnvironmentMap* env) { m_env = env; return this; }

	private:
		ShadowMap* m_map;
		EnvironmentMap* m_env;
	};
}

