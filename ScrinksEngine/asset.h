#pragma once

#include <memory>
#include <map>
#include <functional>
#include <string>
#include "helpers.h"

namespace scrinks::render
{
	class Asset
	{
	public:
		DISABLE_COPY(Asset);

	public:
		inline bool is_loaded() const { return m_loaded; }
		inline bool is_outdated() const { return m_outdated; }
		inline std::uint32_t id() const { return m_id; }

		void tag_outdated() { m_outdated = true; }

	protected:
		Asset(std::uint32_t id);
		bool m_outdated{ false };
		bool m_loaded{ false };
		std::uint32_t m_id{ 0 };
	};

	template <typename T_Descriptor, typename T_Asset>
	class AssetManager
	{
	public:
		DISABLE_COPY_AND_MOVE(AssetManager);

	public:
		std::shared_ptr<T_Asset> load_and_store(const std::string& name, const T_Descriptor& description);
		std::shared_ptr<T_Asset> reload_and_store(const std::string& name, const T_Descriptor& description);
		std::shared_ptr<T_Asset> reload(const std::string& name);
		std::shared_ptr<T_Asset> get(const std::string& name);
		void mark_for_removal(const std::string& name);
		void reload_all();
		void for_each(std::function<void(const std::string& name, std::shared_ptr<T_Asset>)> func);

	public:
		static AssetManager& instance();

	private:
		std::shared_ptr<T_Asset> load(const T_Descriptor& description);

		std::map<std::string, std::pair<T_Descriptor, std::shared_ptr<T_Asset>>> m_store{};

	protected:
		AssetManager() = default;
	};

	template<typename T_Descriptor, typename T_Asset>
	std::shared_ptr<T_Asset> AssetManager<T_Descriptor, T_Asset>::load_and_store(
		const std::string& name,
		const T_Descriptor& description
	)
	{
		const auto asset{ load(description) };

		// Only replace if the asset is actually loaded
		if (asset && asset->is_loaded())
		{
			const auto [idx, success] = m_store.try_emplace(name, std::pair(description, asset));

			if (success)
				return idx->second.second;
		}

		return nullptr;
	}

	template<typename T_Descriptor, typename T_Asset>
	std::shared_ptr<T_Asset> AssetManager<T_Descriptor, T_Asset>::reload_and_store(
		const std::string& name,
		const T_Descriptor& description
	)
	{
		mark_for_removal(name);
		return load_and_store(name, description);
	}

	template<typename T_Descriptor, typename T_Asset>
	std::shared_ptr<T_Asset> AssetManager<T_Descriptor, T_Asset>::reload(const std::string& name)
	{
		const auto& idx = m_store.find(name);

		if (idx != m_store.end())
		{
			std::shared_ptr<T_Asset> reloaded = load(idx->second.first);

			if (reloaded->is_loaded())
			{
				idx->second.second->tag_outdated();
				idx->second.second = reloaded;
			}

			return idx->second.second;
		}

		return nullptr;
	}

	template<typename T_Descriptor, typename T_Asset>
	void AssetManager<T_Descriptor, T_Asset>::reload_all()
	{
		for (auto& [name, entry] : m_store)
			reload(name);
	}

	template<typename T_Descriptor, typename T_Asset>
	void AssetManager<T_Descriptor, T_Asset>::for_each(std::function<void(const std::string& name, std::shared_ptr<T_Asset>)> func)
	{
		for (auto& [name, entry] : m_store)
			func(name, entry.second);
	}

	template<typename T_Descriptor, typename T_Asset>
	std::shared_ptr<T_Asset> AssetManager<T_Descriptor, T_Asset>::get(const std::string& name)
	{
		const auto& idx = m_store.find(name);
		return idx != m_store.end() ? idx->second.second : nullptr;
	}

	template<typename T_Descriptor, typename T_Asset>
	void AssetManager<T_Descriptor, T_Asset>::mark_for_removal(const std::string& name)
	{
		const auto& idx = m_store.find(name);

		if (idx != m_store.end())
		{
			const auto ptr = idx->second.second;
			if (ptr)
			{
				ptr->tag_outdated();
				m_store.erase(idx);
			}
		}
	}

	template<typename T_Descriptor, typename T_Asset>
	AssetManager<T_Descriptor, T_Asset>& AssetManager<T_Descriptor, T_Asset>::instance()
	{
		static AssetManager<T_Descriptor, T_Asset> s_instance;
		return s_instance;
	}
}

