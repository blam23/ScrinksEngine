#include "Shader.h"

#include "debug.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <glm/gtc/type_ptr.hpp>

using namespace scrinks::render;

GLuint compile_shader(const char* code, GLenum type)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, nullptr);
	glCompileShader(shader);

	int success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char log[512];
		glGetShaderInfoLog(shader, 512, nullptr, log);
		std::cerr << "Error compiling shader: '" << log << "'." << std::endl;
	}

	return shader;
}

Shader::Shader(Badge<ShaderManager>, const std::string& name, const std::string& vertexShader, const std::string& fragmentShader)
	: Asset{ name, 0 }
{
	GLuint vertID = compile_shader(vertexShader.c_str(), GL_VERTEX_SHADER);
	GLuint fragID = compile_shader(fragmentShader.c_str(), GL_FRAGMENT_SHADER);

	m_id = glCreateProgram();
	glAttachShader(m_id, vertID);
	glAttachShader(m_id, fragID);
	glLinkProgram(m_id);

	int success;
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (!success)
	{
		char log[512];
		glGetProgramInfoLog(m_id, 512, nullptr, log);
		std::cerr << "Error linking shader: '" << log << "'." << std::endl;
	}

	m_loaded = true;

	// Clean up individual shaders
	glDeleteShader(vertID);
	glDeleteShader(fragID);
}

Shader::~Shader()
{
	if (m_loaded)
		glDeleteShader(m_id);
}

template <>
std::shared_ptr<Shader> ShaderManager::load(const std::string& name, const ShaderDescription& description)
{
	std::string vCode, fCode;
	std::ifstream vFile, fFile;

	vFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vFile.open(description.vertexPath);
		fFile.open(description.fragmentPath);

		std::stringstream vStream, fStream;

		vStream << vFile.rdbuf();
		fStream << fFile.rdbuf();

		vFile.close();
		fFile.close();

		vCode = vStream.str();
		fCode = fStream.str();

		return std::make_shared<Shader>(Badge<ShaderManager>{}, name, vCode, fCode);
	}
	catch (std::ifstream::failure error)
	{
		std::cerr << "Error loading shader <" << description.vertexPath << "," << description.fragmentPath << ">: '" << error.what() << "'." << std::endl;
	}

	return nullptr;
}


void Shader::use_program()
{
	glUseProgram(m_id);

#ifdef DEBUG
	if (m_outdated)
		std::cerr << "Using outdated shader program." << std::endl;
#endif
}

template <>
void Shader::set_param(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

template <>
void Shader::set_param(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

template <>
void Shader::set_param(const std::string& name, std::uint32_t value) const
{
	glUniform1ui(glGetUniformLocation(m_id, name.c_str()), value);
}

template <>
void Shader::set_param(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

template <>
void Shader::set_param(const std::string& name, const std::array<float, 2>& value) const
{
	glUniform2f(glGetUniformLocation(m_id, name.c_str()), value[0], value[1]);
}
template <>
void Shader::set_param(const std::string& name, const glm::vec2& value) const
{
	glUniform2f(glGetUniformLocation(m_id, name.c_str()), value[0], value[1]);
}

template <>
void Shader::set_param(const std::string& name, const std::array<int, 2>& value) const
{
	glUniform2i(glGetUniformLocation(m_id, name.c_str()), value[0], value[1]);
}

template <>
void Shader::set_param(const std::string& name, const std::array<float, 3>& value) const
{
	glUniform3f(glGetUniformLocation(m_id, name.c_str()), value[0], value[1], value[2]);
}
template <>
void Shader::set_param(const std::string& name, const glm::vec3& value) const
{
	glUniform3f(glGetUniformLocation(m_id, name.c_str()), value[0], value[1], value[2]);
}
template <>
void Shader::set_param(const std::string& name, glm::vec3 value) const
{
	glUniform3f(glGetUniformLocation(m_id, name.c_str()), value[0], value[1], value[2]);
}

template <>
void Shader::set_param(const std::string& name, const std::array<int, 3>& value) const
{
	glUniform3i(glGetUniformLocation(m_id, name.c_str()), value[0], value[1], value[2]);
}

template <>
void Shader::set_param(const std::string& name, const std::array<float, 4>& value) const
{
	glUniform4f(glGetUniformLocation(m_id, name.c_str()), value[0], value[1], value[2], value[3]);
}
template <>
void Shader::set_param(const std::string& name, const std::array<int, 4>& value) const
{
	glUniform4i(glGetUniformLocation(m_id, name.c_str()), value[0], value[1], value[2], value[3]);
}
template <>
void Shader::set_param(const std::string& name, const glm::vec4& value) const
{
	glUniform4f(glGetUniformLocation(m_id, name.c_str()), value[0], value[1], value[2], value[3]);
}

template <>
void Shader::set_param(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
template <>
void Shader::set_param(const std::string& name, glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_test_param(const std::string& name) const
{
	set_param(name, scrinks::debug::get_test_float(name));
}


