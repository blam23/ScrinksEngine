#include "render_target.h"

#include <iostream>

using namespace scrinks::render;

scrinks::render::RenderTarget::RenderTarget(const std::string& name, BufferFormat format, GLint width, GLint height, GLenum attachment)
{
	m_buffer = BufferManager::reload_and_store(name, { format, attachment, width, height });

	glGenFramebuffers(1, &m_bufID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_bufID);

	glFramebufferTexture2D(GL_FRAMEBUFFER, m_buffer->attachment(), GL_TEXTURE_2D, m_buffer->id(), 0);

	// don't need to set this up for depth buffer
	if (attachment != GL_DEPTH_ATTACHMENT)
	{
		GLenum attachments[1]{ attachment };
		glDrawBuffers(1, attachments);
	}

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

