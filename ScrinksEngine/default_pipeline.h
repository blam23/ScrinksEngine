#pragma once

#include "pipeline.h"
#include "pass_geometry.h"
#include "pass_light.h"
#include "pass_ssao.h"
#include "pass_post_processing.h"
#include "pass_shadow_map.h"
#include "pass_environment_map.h"
#include "pass_sky_map.h"

namespace scrinks::render
{
	void default_pipeline()
	{
		pass::ShadowMap* map = Pipeline::register_pass<pass::ShadowMap>()
			->init(4096);

		pass::EnvironmentMap* env = Pipeline::register_pass<pass::EnvironmentMap>()
			->init(512);

		Pipeline::register_pass<pass::Geometry>()
			->init();

		Pipeline::register_pass<pass::SkyMap>()
			->init();

		Pipeline::register_pass<pass::SSAO>()
			->init();

		Pipeline::register_pass<pass::SSAOBlur>()
			->init();

		Pipeline::register_pass<pass::DeferredLighting>()
			->set_shadow_map(map)
			->set_env_map(env)
			->init();

		Pipeline::register_pass<pass::PostProcessing>()
			->init();
	}
}

