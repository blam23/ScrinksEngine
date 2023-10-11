#pragma once

#include "helpers.h"
#include "asset.h"
#include <cstdint>
#include <string_view>

#include <gl/gl3w.h>

namespace scrinks::render
{
	class Texture;

	enum class TextureType
	{
		SDR,
		HDR
	};

	struct TextureDescription
	{
		const std::string path;
		const TextureType type;

		TextureDescription(const std::string& path, const TextureType type = TextureType::SDR)
			: path{ path }, type{ type }
		{
		}
		
		TextureDescription(const char* path, const TextureType type = TextureType::SDR)
			: path{ path }, type{ type }
		{
		}
	};

	using TextureManager = core::AssetManager<TextureDescription, Texture>;

	class Texture : public core::Asset
	{
	public:
		Texture(Badge<TextureManager>, const std::string& name, uint32_t id, GLuint width, GLuint height, GLenum format);
		~Texture();

		inline GLuint width() const { return m_width; }
		inline GLuint height() const { return m_height; }
		inline GLuint format() const { return m_format; }

	public:
		void bind(GLenum unit) const;

	private:
		GLuint m_width, m_height, m_format;
	};
}
