#pragma once

#include <string>
#include <map>
#include <glm/ext/vector_float3.hpp>

namespace scrinks::debug
{
	std::map<std::string, float>& get_all_test_floats();
	float get_test_float(const std::string& name, float defaultValue = 0);

	std::map<std::string, glm::vec3>& get_all_test_dirs();
	const glm::vec3& get_test_dir(const std::string& name);
}
