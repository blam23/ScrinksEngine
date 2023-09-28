#include "gbuffer.h"
#include "glm/glm.hpp"
#include <map>
#include <iostream>

using namespace scrinks::render;

std::map<BufferFormat, Buffer::Metadata> s_data{
	{ BufferFormat::rgb,     { GL_RGB,                GL_RGB,             GL_UNSIGNED_BYTE } },
	{ BufferFormat::rgba,    { GL_RGBA,               GL_RGBA,            GL_UNSIGNED_BYTE } },
	{ BufferFormat::rgb16f,  { GL_RGB16F,             GL_RGB,             GL_FLOAT } },
	{ BufferFormat::rgba16f, { GL_RGBA16F,            GL_RGBA,            GL_FLOAT } },
	{ BufferFormat::depth,   { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT } },
	{ BufferFormat::red,     { GL_RED,                GL_RED,             GL_UNSIGNED_BYTE} },
};

void Buffer::bind(GLenum slot, const std::string bufferName)
{
	const auto buffer{ BufferManager::instance().get(bufferName) };

	if (!buffer)
	{
		std::cerr << "Attempt to activate null buffer '" << bufferName << "'." << std::endl;
		return;
	}

	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, buffer->id());
}

Buffer::Buffer(Badge<BufferManager>, BufferDescriptor descriptor)
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

Buffer::~Buffer()
{
	if (m_id != 0)
		glDeleteTextures(1, &m_id);
}

Buffer::Metadata Buffer::get_format_meta_data(BufferFormat format)
{
	return s_data[format];
}

void bind(const std::shared_ptr<Buffer>& buffer)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, buffer->attachment(), GL_TEXTURE_2D, buffer->id(), 0);
}

template <>
std::shared_ptr<Buffer> BufferManager::load(const BufferDescriptor& description)
{
	return std::make_shared<Buffer>(Badge<BufferManager>{}, description);
}

void GBuffer::setup_buffers()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);

	for (const auto& buffer : m_buffers)
		bind(buffer);

	std::vector<uint32_t> attachments;
	for (GLuint i = 0; i < m_colorBufferCount; i++)
		attachments.push_back(GL_COLOR_ATTACHMENT0 + i);

	glDrawBuffers(m_colorBufferCount, attachments.data());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Error creating GBuffer." << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::add_buffer(const std::string& name, BufferFormat format, GLuint attachment)
{
	GLuint calcAttachment
	{
		attachment == GL_COLOR_ATTACHMENT0
			? GL_COLOR_ATTACHMENT0 + m_colorBufferCount++
			: attachment
	};

	m_buffers.push_back(BufferManager::instance().reload_and_store(name, { format, calcAttachment, m_width, m_height }));
}

GBuffer::GBuffer(GLsizei width, GLsizei height)
	: m_buffers{}
	, m_width{ width }
	, m_height{ height }
{
	add_buffer("position",  BufferFormat::rgb16f);
	add_buffer("normal",    BufferFormat::rgb16f);
	add_buffer("albedo",    BufferFormat::rgba);
	add_buffer("depth",     BufferFormat::depth, GL_DEPTH_ATTACHMENT);

	glGenFramebuffers(1, &m_id);
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

