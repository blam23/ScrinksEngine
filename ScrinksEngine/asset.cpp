#include "asset.h"

#include <map>

using namespace scrinks::core;

Asset::Asset(const std::string& name, std::uint32_t id)
	: m_name{ name }
	, m_id{ id }
{
}
