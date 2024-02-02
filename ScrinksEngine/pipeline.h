#pragma once

#include "helpers.h"
#include "gbuffer.h"
#include "camera.h"
#include "shader.h"
#include <memory>
#include <typeindex>

namespace scrinks::render
{
	class RenderPass;

	class Pipeline
	{
	public:
		Pipeline() = delete;

	public:
		static void draw(float interpolation);
		static void resize(GLsizei width, GLsizei height);
		static void resize_if_needed(GLsizei width, GLsizei height);
		static void force_recreate();
		static void fullscreen_quad();
		static void unit_cube();

		template <typename T_Pass>
		static T_Pass* register_pass(std::string&& name);

		template <typename T_Pass>
		static T_Pass* get_pass(const std::string& name);

	public:
		static GBuffer* gbuffer();
		static GLsizei view_width() { return s_viewWidth; }
		static GLsizei view_height() { return s_viewHeight; }
		static FPSCamera& camera() { return s_camera; }
		static const glm::mat4& projection() { return s_projection; }

	private:
		static std::unique_ptr<GBuffer> s_gbuffer;
		static std::vector<std::unique_ptr<RenderPass>> s_passes;
		static inline std::map<std::string, RenderPass*> s_pass_map{};
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
		virtual void draw(float interpolate) = 0;

	protected:
		RenderPass() = default;

	private:
		virtual void resize(GLsizei width, GLsizei height) = 0;

		friend Pipeline;
	};

	template <typename T_Pass>
	T_Pass* Pipeline::register_pass(std::string&& name)
	{
		auto [itr, inserted] = s_pass_map.emplace(std::move(name), nullptr);

		if (inserted)
		{
			s_passes.push_back(std::make_unique<T_Pass>());
			size_t index{ s_passes.size() - 1 };
			auto ptr{ s_passes[index].get() };
			itr->second = ptr;
			return (T_Pass*)ptr;
		}

		return nullptr;
	}

	template <typename T_Pass>
	T_Pass* Pipeline::get_pass(const std::string& name)
	{
		auto itr = s_pass_map.find(name);

		if (itr != s_pass_map.end())
			return (T_Pass*)itr->second;

		return nullptr;
	}

	struct AutoResetDepthFunc
	{
	public:
		DISABLE_COPY_AND_MOVE(AutoResetDepthFunc);

		AutoResetDepthFunc(GLenum depthFunc)
		{
			glDepthFunc(depthFunc);
		}

		~AutoResetDepthFunc()
		{
			glDepthFunc(GL_GEQUAL); // default
		}
	};
}
