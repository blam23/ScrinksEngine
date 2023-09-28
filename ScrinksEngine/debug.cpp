#include "debug.h"

std::map<std::string, float> s_testFloats{};
std::map<std::string, glm::vec3> s_testDirs{};

std::map<std::string, float>& scrinks::debug::get_all_test_floats()
{
    return s_testFloats;
}

std::map<std::string, glm::vec3>& scrinks::debug::get_all_test_dirs()
{
    return s_testDirs;
}

float scrinks::debug::get_test_float(const std::string& name, float defaultValue)
{
    auto idx = s_testFloats.find(name);
    if (idx == s_testFloats.end())
    {
        s_testFloats[name] = defaultValue;
        return s_testFloats[name];
    }

    return idx->second;
}

const glm::vec3& scrinks::debug::get_test_dir(const std::string& name)
{
    auto idx = s_testDirs.find(name);
    if (idx == s_testDirs.end())
    {
        s_testDirs[name] = glm::vec3(0.0f);
        return s_testDirs[name];
    }

    return idx->second;
}

