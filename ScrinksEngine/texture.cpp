#include "texture.h"

#include <type_traits>
#include <iostream>

using namespace scrinks::render;

#pragma warning (push, 3)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma warning (pop)


Texture::Texture(Badge<TextureManager>, const std::string& name, GLuint id, GLuint width, GLuint height, GLuint channels)
	: Asset{ name, id }, m_width{ width }, m_height{ height }, m_channels{ channels }
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
std::shared_ptr<Texture> TextureManager::load(const std::string& name, const TextureDescription& path)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (data)
	{
		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (channels == 1)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

		return std::make_shared<Texture>(Badge<TextureManager>{}, name, id, width, height, channels);
	}
	stbi_image_free(data);

	std::cerr << "Failed to load texture: '" << path << "'. Reason: " << stbi_failure_reason() << std::endl;
	return nullptr;
}

