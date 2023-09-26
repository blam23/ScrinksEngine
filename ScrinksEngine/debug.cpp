#include "debug.h"


std::map<std::string, float> s_testFloats{};

void scrinks::debug::register_test_float(const std::string& name, float initialValue)
{
    s_testFloats[name] = initialValue;
}

std::map<std::string, float>& scrinks::debug::get_all_test_floats()
{
    return s_testFloats;
}

float* scrinks::debug::get_test_float(const std::string& name)
{
    return &s_testFloats[name];
}

