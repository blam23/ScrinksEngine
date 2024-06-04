#include "text_file_asset.h"

#include <fstream>
#include "spdlog/spdlog.h"

using namespace scrinks::core;

TextFile::TextFile(Badge<TextFileManager>, const std::string& name, const std::string& content)
	: Asset{ name, 0 }
	, m_contents{ content }
{
	m_loaded = true;
}

auto scrinks::core::TextFile::template_contents(const std::map<std::string, std::string>& params) const -> std::string
{
	auto content{ m_contents };

	// TODO: this could be more efficient
	for (const auto& [find, replace] : params)
	{
		const auto itr = content.find(find);
		if (itr != std::string::npos)
			content = content.replace(itr, find.length(), replace);
	}

	return content;
}

template <>
std::shared_ptr<TextFile> TextFileManager::load(const std::string& name, const TextFileDescription& description)
{
	std::string content;
	std::ifstream file;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		file.open(description);

		std::stringstream stream;
		stream << file.rdbuf();

		file.close();

		content = stream.str();
		return std::make_shared<TextFile>(Badge<TextFileManager>{}, name, content);
	}
	catch (const std::ifstream::failure& error)
	{
		spdlog::error("Error loading text file <{}>: {}", description, error.what());
	}

	return nullptr;
}

void scrinks::core::write_text_file(const std::string& path, const std::string& contents)
{
	try
	{
		std::fstream file{ path, std::fstream::out };
		file << contents;
		file.close();
	}
	catch (const std::ifstream::failure& error)
	{
		spdlog::error("Error writing text file <{}>: {}", path, error.what());
	}
}
