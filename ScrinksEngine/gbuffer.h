#pragma once

#include <gl/gl3w.h>
#include <GLFW/glfw3.h>

#include "helpers.h"
#include "asset.h"

namespace scrinks::render
{
	class Buffer;

	enum class BufferFormat
	{
		rgb,
		rgb16f,
		rgba16f,
		rgba,
		depth
	};

	struct BufferDescriptor
	{
		BufferFormat format;
		GLenum attachment;
		GLint width;
		GLint height;
	};
	using BufferManager = AssetManager<BufferDescriptor, Buffer>;

	class Buffer : public Asset
	{
	public:
		DISABLE_COPY(Buffer);

	public:
		struct Metadata
		{
			GLint internalFormat;
			GLint format;
			GLenum type;
		};

	public:
		static Metadata get_format_meta_data(BufferFormat format);

	public:
		Buffer(Badge<BufferManager>, BufferDescriptor desc);
		~Buffer();

		inline GLuint attachment() const { return m_attachment; }
		inline GLuint width() const      { return m_width; }
		inline GLuint height() const     { return m_height; }
			
	private:
		GLenum m_attachment{ 0 };
		GLsizei m_width{ 0 }, m_height{ 0 };
	};

	class GBuffer
	{
	public:
		DISABLE_COPY(GBuffer);

	public:
		GBuffer(GLsizei width, GLsizei height);
		~GBuffer();

		void bind_write();
		void bind_read();
		void bind_textures();

	private:
		GLuint m_id;
		std::shared_ptr<Buffer> m_posTexture;
		std::shared_ptr<Buffer> m_normalTexture;
		std::shared_ptr<Buffer> m_albedoTexture;
		std::shared_ptr<Buffer> m_depthTexture;
	};
}
