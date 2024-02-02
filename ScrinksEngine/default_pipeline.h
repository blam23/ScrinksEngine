#pragma once

#include "pipeline.h"
#include "pass_geometry.h"
#include "pass_light.h"
#include "pass_ssao.h"
#include "pass_post_processing.h"
#include "pass_shadow_map.h"
#include "pass_environment_map.h"
#include "pass_sky_map.h"
#include "sprite_renderer.h"

namespace scrinks::render
{
	void default_pipeline()
	{
		auto map = Pipeline::register_pass<pass::ShadowMap>("shadow_map")
			->init(4096);

		auto env = Pipeline::register_pass<pass::EnvironmentMap>("environment_map")
			->init(512);

		Pipeline::register_pass<pass::Geometry>("geometry")
			->init();

		Pipeline::register_pass<pass::SkyMap>("sky_map")
			->init();

		Pipeline::register_pass<pass::SSAO>("ssao")
			->init();

		Pipeline::register_pass<pass::SSAOBlur>("ssao_blur")
			->init();

		Pipeline::register_pass<pass::DeferredLighting>("lighting")
			->set_shadow_map(map)
			->set_env_map(env)
			->init();

		Pipeline::register_pass<pass::PostProcessing>("post_processing")
			->init();

		Pipeline::register_pass<pass::SpriteRender>("sprite_renderer")
			->init("sprites");
	}
}

