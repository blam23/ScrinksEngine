#include "mesh.h"

using namespace scrinks::render;

void Mesh::_cleanup() noexcept
{
	if (m_vbo != 0)
		glDeleteFramebuffers(1, &m_vbo);
	if (m_ebo != 0)
		glDeleteFramebuffers(1, &m_ebo);
	if (m_vao != 0)
		glDeleteVertexArrays(1, &m_vao);
}

void Mesh::_move(Mesh& target, Mesh& other) noexcept
{
	target.m_vertices = std::move(other.m_vertices);
	target.m_indices = std::move(other.m_indices);
	target.m_textures = std::move(other.m_textures);

	target.m_vbo = other.m_vbo;
	target.m_ebo = other.m_ebo;
	target.m_vao = other.m_vao;

	other.m_vbo = 0;
	other.m_ebo = 0;
	other.m_vao = 0;
}

Mesh::Mesh(Mesh&& other) noexcept
{
	_move(*this, other);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (this != &other)
	{
		_cleanup();
		_move(*this, other);
	}

	return *this;
}

Mesh::Mesh
(
	std::vector<Vertex>&& vertices,
	std::vector<GLuint>&& indices,
	TextureVector&& textures
)
	: m_vertices{ std::move(vertices) }
	, m_indices{ std::move(indices) }
	, m_textures{ std::move(textures) }
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, (GLsizei)(m_vertices.size() * sizeof(Vertex)), m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)(m_indices.size() * sizeof(GLuint)), m_indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glBindVertexArray(0);
}

scrinks::render::Mesh::~Mesh()
{
	_cleanup();
}

void Mesh::draw(std::shared_ptr<Shader> shader) const
{
	for (std::uint32_t i = 0; i < m_textures.size(); i++)
	{
		shader->set_param("tex" + std::to_string(i), i);
		m_textures[i]->bind(GL_TEXTURE0 + i);
	}
	glActiveTexture(GL_TEXTURE1);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
