#pragma once

#include "pipeline.h"
#include "pass_geometry.h"
#include "pass_light.h"
#include "pass_ssao.h"
#include "pass_post_processing.h"

namespace scrinks::render
{
	void default_pipeline()
	{
		render::Pipeline::register_pass<render::pass::Geometry>();
		render::Pipeline::register_pass<render::pass::SSAO>();
		render::Pipeline::register_pass<render::pass::SSAOBlur>();
		render::Pipeline::register_pass<render::pass::DeferredLighting>();
		render::Pipeline::register_pass<render::pass::PostProcessing>();
	}
}

