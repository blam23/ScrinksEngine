#pragma once

#include "helpers.h"
#include "texture.h"
#include "shader.h"

#include <vector>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <gl/gl3w.h>

namespace scrinks::render
{
	class Mesh;
	using MeshDescription = std::string;
	using TextureVector = std::vector<std::shared_ptr<Texture>>;

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
	};

	class Mesh
	{
	public:
		DISABLE_COPY(Mesh);
		Mesh(Mesh&&) noexcept;
		Mesh& operator=(Mesh&&) noexcept;

	public:
		Mesh(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices, TextureVector&& textures);
		~Mesh();
		void draw(std::shared_ptr<Shader> shader) const;

	private:
		static void _move(Mesh& target, Mesh& other) noexcept;
		void _cleanup() noexcept;

	private:
		GLuint m_vao;
		GLuint m_vbo;
		GLuint m_ebo;

		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
		TextureVector m_textures;
	};
}

