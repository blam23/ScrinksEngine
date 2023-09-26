#pragma once

#include <gl/gl3w.h>

#include "helpers.h"
#include "asset.h"

#include <cstdint>
#include <string>
#include <map>
#include <optional>
#include <memory>

namespace scrinks::render
{
	class Shader;

	struct ShaderDescription
	{
		std::string vertexPath;
		std::string fragmentPath;
	};

	using ShaderManager = AssetManager<ShaderDescription, Shader>;

	class Shader : public Asset
	{
	public:
		Shader(Badge<ShaderManager>, const std::string& vertexShader, const std::string& fragmentShader);
		~Shader();

	public:
		void use_program();

		template <typename T>
		void set_param(const std::string& name, T value) const;

	public:
		void set_test_param(const std::string& name) const;
	};
}
