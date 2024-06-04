#pragma once

#include "helpers.h"
#include "asset.h"
#include <map>

namespace scrinks::core
{
	class TextFile;

	using TextFileDescription = std::string;
	using TextFileManager = core::AssetManager<TextFileDescription, TextFile>;

	class TextFile : public core::Asset
	{
	public:
		TextFile(Badge<TextFileManager>, const std::string& file, const std::string& content);

	public:
		auto& get_contents() const { return m_contents; }
		auto template_contents(const std::map<std::string, std::string>& params) const -> std::string;

	private:
		std::string m_contents;
	};

	void write_text_file(const std::string& path, const std::string& contents);
}

