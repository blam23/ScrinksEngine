#include "render_target.h"

#include <iostream>

using namespace scrinks::render;

scrinks::render::RenderTarget::RenderTarget(const std::string name, BufferFormat format, GLint width, GLint height)
{
	m_buffer = BufferManager::instance().reload_and_store(name, { format, GL_COLOR_ATTACHMENT0, width, height });

	glGenFramebuffers(1, &m_bufID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_bufID);

	glFramebufferTexture2D(GL_FRAMEBUFFER, m_buffer->attachment(), GL_TEXTURE_2D, m_buffer->id(), 0);

	std::uint32_t attachments[1]{ GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Error creating Render Target." << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::bind_write()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_bufID);
}

void RenderTarget::bind_read()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_bufID);
}

