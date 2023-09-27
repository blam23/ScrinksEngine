#pragma once

#include "helpers.h"
#include "gbuffer.h"
#include "camera.h"
#include <memory>

namespace scrinks::render
{
	class RenderPass;
	using RenderPassId = std::size_t;

	class Pipeline
	{
	public:
		Pipeline() = delete;

	public:
		static void draw();
		static void resize(GLsizei width, GLsizei height);
		static void resize_if_needed(GLsizei width, GLsizei height);
		static void force_recreate();
		static void fullscreen_quad();

		template <typename T_Pass>
		static RenderPassId register_pass();

	public:
		static GBuffer* gbuffer();
		static GLsizei view_width() { return s_viewWidth; }
		static GLsizei view_height() { return s_viewHeight; }
		static FPSCamera& camera() { return s_camera; }
		static const glm::mat4& projection() { return s_projection; }

	private:
		static std::unique_ptr<GBuffer> s_gbuffer;
		static std::vector<std::unique_ptr<RenderPass>> s_passes;
		static void setup(GLsizei width, GLsizei height);
		static void handle_resize();

		static GLsizei s_viewWidth;
		static GLsizei s_viewHeight;
		static FPSCamera s_camera;
		static glm::mat4 s_projection;
	};

	class RenderPass
	{
	public:
		DISABLE_COPY_AND_MOVE(RenderPass);
		virtual ~RenderPass() {}

	public:
		virtual void init() { m_initialised = true; }
		virtual void draw() = 0;

	public:
		static constexpr RenderPassId InvalidPassId{ (std::numeric_limits<std::uint32_t>::max)() };

	protected:
		RenderPass() = default;
		bool m_initialised{ false };

	private:
		virtual void resize(GLsizei width, GLsizei height) = 0;

		RenderPassId m_id{ InvalidPassId };
		friend Pipeline;
	};

	template <typename T_Pass>
	RenderPassId Pipeline::register_pass()
	{
		s_passes.push_back(std::make_unique<T_Pass>());
		size_t index{ s_passes.size() - 1 };
		s_passes[index]->init();
		return index;
	}
}
