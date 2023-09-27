#pragma once

#include "pipeline.h"
#include "pass_geometry.h"
#include "pass_light.h"
#include "pass_ssao.h"

namespace scrinks::render
{
	void default_pipeline()
	{
		render::Pipeline::register_pass<render::pass::Geometry>();
		render::Pipeline::register_pass<render::pass::SSAO>();
		render::Pipeline::register_pass<render::pass::DeferredLighting>();
	}
}

