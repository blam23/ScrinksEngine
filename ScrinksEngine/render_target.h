#pragma once
#pragma once

#include <gl/gl3w.h>
#include <GLFW/glfw3.h>

#include "helpers.h"
#include "gbuffer.h"

namespace scrinks::render
{
	class RenderTarget
	{
	public:
		DISABLE_COPY(RenderTarget);

	public:
		RenderTarget(const std::string name, BufferFormat format, GLint width, GLint height, GLenum attachment = GL_COLOR_ATTACHMENT0);

	public:
		void bind_write();
		void bind_read();
		inline GLuint tex_id() const { return m_buffer->id(); }
		inline GLuint id() const { return m_bufID; }

	private:
		GLuint m_bufID{ 0 };
		std::shared_ptr<Buffer> m_buffer;
	};
}
