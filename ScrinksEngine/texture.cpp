#include "texture.h"

#include <type_traits>
#include <iostream>

using namespace scrinks::render;

#pragma warning (push, 3)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma warning (pop)


Texture::Texture(Badge<TextureManager>, const std::string& name, GLuint id, GLuint width, GLuint height, GLenum format)
	: Asset{ name, id }, m_width{ width }, m_height{ height }, m_format{ format }
{
	m_loaded = true;
}

Texture::~Texture()
{
	if (m_loaded)
	{
		glDeleteTextures(1, &m_id);
	}
}

void Texture::bind(GLenum unit) const
{
	glActiveTexture(unit);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

template <>
std::shared_ptr<Texture> TextureManager::load(const std::string& name, const TextureDescription& desc)
{
	stbi_set_flip_vertically_on_load(false);

	int width, height, channels;
	
	void* data{ nullptr };
	GLenum dataType{ GL_UNSIGNED_BYTE };
	switch (desc.type)
	{
		case TextureType::HDR:
			data = stbi_loadf(desc.path.c_str(), &width, &height, &channels, 0);
			dataType = GL_FLOAT;
			break;
		case TextureType::SDR:
			data = stbi_load(desc.path.c_str(), &width, &height, &channels, 0);
			dataType = GL_UNSIGNED_BYTE;
			break;
		default:
			std::cerr << "Failed to load texture: '" << desc.path << "'. Unable to load unknown texture type '" << (std::uint32_t)desc.type << "'." << std::endl;
			return nullptr;
	}

	if (data)
	{
		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		// TODO: this is ugly as hell, come on lad, fix this
		if (desc.type == TextureType::HDR)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (channels == 1)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, dataType, data);

			if (channels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, dataType, data);

			if (channels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, dataType, data);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			if (channels == 1)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, dataType, data);

			if (channels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, dataType, data);

			if (channels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, dataType, data);

			glGenerateMipmap(GL_TEXTURE_2D);
		}

		stbi_image_free(data);

		return std::make_shared<Texture>(Badge<TextureManager>{}, name, id, width, height, channels);
	}
	stbi_image_free(data);

	std::cerr << "Failed to load texture: '" << desc.path << "'. Reason: " << stbi_failure_reason() << std::endl;
	return nullptr;
}

