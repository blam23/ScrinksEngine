#pragma once

#include "helpers.h"
#include "shader.h"
#include "pipeline.h"
#include <vector>
#include <type_traits>
#include <iostream>

namespace scrinks::render
{
	template <typename T_Instance>
	class InstanceRenderer
	{
		static_assert(std::is_trivially_copyable_v<T_Instance> == true);

	public:
		DISABLE_COPY_AND_MOVE(InstanceRenderer);

		InstanceRenderer(std::shared_ptr<Shader> shader, size_t count);
		virtual ~InstanceRenderer();

	public:
		static constexpr size_t INSTANCE_SIZE = 3;

	public:
		void bind_vertex_data(const std::vector<float>& vertices, GLint stride, GLenum drawType);
		virtual void bind_instance_data();
		virtual void render();
		virtual void setup_shader();

		virtual void update_instance(std::size_t index, const T_Instance& data);
		size_t push(const T_Instance& data);

	public:
		std::size_t m_count{ 0 };

	protected:
		void set_shader(std::shared_ptr<Shader>& shader) { m_shader = shader; }

		std::vector<float> m_data{};
		std::size_t m_dataUsedLength{ 0 };
		GLuint m_vao{};
		GLuint m_vbo{};

		std::shared_ptr<Shader> m_shader;
		GLenum m_drawType{ GL_TRIANGLE_STRIP };
		GLint m_stride{ 4 };
	};

	template<typename T_Instance>
	inline InstanceRenderer<T_Instance>::InstanceRenderer(std::shared_ptr<Shader> shader, size_t count)
		: m_shader{ shader }
	{
		m_data.resize(count * INSTANCE_SIZE);

		glGenBuffers(1, &m_vbo);
		glGenVertexArrays(1, &m_vao);
	}

	template<typename T_Instance>
	inline InstanceRenderer<T_Instance>::~InstanceRenderer()
	{
		glDeleteBuffers(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
	}

	template<typename T_Instance>
	inline void InstanceRenderer<T_Instance>::bind_vertex_data(const std::vector<float>& vertices, GLint stride, GLenum drawType)
	{
		m_stride = stride;
		m_drawType = drawType;
		GLuint vbo;
		glBindVertexArray(m_vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, m_stride, GL_FLOAT, GL_FALSE, m_stride * sizeof(float), 0);
	}

	template<typename T_Instance>
	inline void InstanceRenderer<T_Instance>::bind_instance_data()
	{
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_data.size(), m_data.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexAttribPointer(1, INSTANCE_SIZE, GL_FLOAT, GL_FALSE, INSTANCE_SIZE * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(1, 1);
		glBindVertexArray(0);
	}

	template<typename T_Instance>
	inline void InstanceRenderer<T_Instance>::render()
	{
		if (m_shader->is_outdated())
			m_shader = ShaderManager::get(m_shader->asset_name());

		m_shader->use_program();
		setup_shader();

		glBindVertexArray(m_vao);
		glDrawArraysInstanced(m_drawType, 0, m_stride, (GLsizei)m_count);
		glBindVertexArray(0);
	}

	template<typename T_Instance>
	inline void InstanceRenderer<T_Instance>::setup_shader()
	{
		m_shader->set_param<const glm::mat4&>("view", Pipeline::camera().view());
		m_shader->set_param<const glm::mat4&>("projection", Pipeline::projection());
	}

	template<typename T_Instance>
	inline void InstanceRenderer<T_Instance>::update_instance(std::size_t index, const T_Instance& data)
	{
		size_t dataIndex{ index * INSTANCE_SIZE };
		memcpy_s(&m_data[dataIndex], m_data.size() * sizeof(float), &data, INSTANCE_SIZE * sizeof(float));
	}

	template<typename T_Instance>
	inline size_t InstanceRenderer<T_Instance>::push(const T_Instance& data)
	{
		if (m_dataUsedLength == m_data.size())
		{
			std::cerr << "Instance buffer is full!" << std::endl;
			return (std::numeric_limits<std::size_t>::max)();
		}

		update_instance(m_count, data);
		m_dataUsedLength += INSTANCE_SIZE;
		return m_count++;
	}
}
