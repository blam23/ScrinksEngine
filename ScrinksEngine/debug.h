#pragma once

#include <string>
#include <map>

namespace scrinks::debug
{
	void register_test_float(const std::string& name, float initialValue = 0.0f);
	std::map<std::string, float>& get_all_test_floats();
	float* get_test_float(const std::string& name);
}
