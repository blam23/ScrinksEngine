#pragma once

#include "helpers.h"
#include "asset.h"
#include <cstdint>

#include <gl/gl3w.h>

namespace scrinks::render
{
	class Texture;
	using TextureDescription = std::string;
	using TextureManager = core::AssetManager<TextureDescription, Texture>;

	class Texture : public core::Asset
	{
	public:
		Texture(Badge<TextureManager>, const std::string& name, uint32_t id, GLuint width, GLuint height, GLuint channels);
		~Texture();

		inline GLuint width() const { return m_width; }
		inline GLuint height() const { return m_height; }
		inline GLuint channels() const { return m_channels; }

	public:
		void bind(GLenum unit) const;

	private:
		GLuint m_width, m_height, m_channels;
	};
}
