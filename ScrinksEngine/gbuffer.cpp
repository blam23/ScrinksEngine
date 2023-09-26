#include "gbuffer.h"
#include "glm/glm.hpp"
#include <map>
#include <iostream>

using namespace scrinks::render;

std::map<BufferFormat, Buffer::Metadata> s_data{
	{ BufferFormat::rgb,     { GL_RGB,                GL_RGB,             GL_UNSIGNED_BYTE }},
	{ BufferFormat::rgba,    { GL_RGBA,               GL_RGBA,            GL_UNSIGNED_BYTE }},
	{ BufferFormat::rgb16f,  { GL_RGB16F,             GL_RGB,             GL_FLOAT } },
	{ BufferFormat::rgba16f, { GL_RGBA16F,            GL_RGBA,            GL_FLOAT } },
	{ BufferFormat::depth,   { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT }},
};

scrinks::render::Buffer::Buffer(Badge<BufferManager>, BufferDescriptor descriptor)
	: m_attachment{ descriptor.attachment }
	, m_width{ descriptor.width }
	, m_height{ descriptor.height }
	, Asset{ 0 }
{
	const auto& metadata = get_format_meta_data(descriptor.format);

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, metadata.internalFormat, descriptor.width, descriptor.height, 0, metadata.format, metadata.type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_loaded = true;
}

scrinks::render::Buffer::~Buffer()
{
	if (m_id != 0)
		glDeleteTextures(1, &m_id);
}

Buffer::Metadata Buffer::get_format_meta_data(BufferFormat format)
{
	return s_data[format];
}

void bind(const Buffer& buffer)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, buffer.attachment(), GL_TEXTURE_2D, buffer.id(), 0);
}

template <>
std::shared_ptr<Buffer> BufferManager::load(const BufferDescriptor& description)
{
	return std::make_shared<Buffer>(Badge<BufferManager>{}, description);
}

GBuffer::GBuffer(GLsizei width, GLsizei height)
{
	m_posTexture    = BufferManager::instance().reload_and_store("position", { BufferFormat::rgb16f, GL_COLOR_ATTACHMENT0, width, height });
	m_normalTexture = BufferManager::instance().reload_and_store("normal",   { BufferFormat::rgb16f, GL_COLOR_ATTACHMENT1, width, height });
	m_albedoTexture = BufferManager::instance().reload_and_store("albedo",   { BufferFormat::rgba,   GL_COLOR_ATTACHMENT2, width, height });
	m_depthTexture  = BufferManager::instance().reload_and_store("depth",    { BufferFormat::depth,  GL_DEPTH_ATTACHMENT,  width, height });

	glGenFramebuffers(1, &m_id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);

	bind(*m_posTexture);
	bind(*m_normalTexture);
	bind(*m_albedoTexture);
	bind(*m_depthTexture);

	std::uint32_t attachments[3]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Error creating GBuffer." << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

scrinks::render::GBuffer::~GBuffer()
{
	if (m_id != 0)
		glDeleteFramebuffers(1, &m_id);
}

void GBuffer::bind_write()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
}

void GBuffer::bind_read()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
}

void GBuffer::bind_textures()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_posTexture->id());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture->id());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_albedoTexture->id());
}

