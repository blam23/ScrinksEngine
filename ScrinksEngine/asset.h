#pragma once

#include <memory>
#include <map>
#include <functional>
#include <string>
#include "helpers.h"

namespace scrinks::core
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
		const std::string& asset_name() const { return m_name; }

	protected:
		Asset(const std::string& name, std::uint32_t id);
		bool m_outdated{ false };
		bool m_loaded{ false };
		std::uint32_t m_id{ 0 };
		const std::string m_name;
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
		void for_each_name(std::function<void(const std::string& name)> func);

	public:
		static AssetManager& instance();

	protected:
		std::shared_ptr<T_Asset> load(const std::string& name, const T_Descriptor& description);
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
		const auto itr{ m_store.find(name) };
		if (itr != m_store.end())
			return itr->second.second;

		const auto asset{ load(name, description) };

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
			std::shared_ptr<T_Asset> reloaded = load(name, idx->second.first);

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
	void AssetManager<T_Descriptor, T_Asset>::for_each_name(std::function<void(const std::string& name)> func)
	{
		for (auto& [name, _] : m_store)
			func(name);
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

	template <typename T_Descriptor, typename T_RawAsset, typename T_Asset>
	class GenerativeAssetManager : public AssetManager<T_Descriptor, T_Asset>
	{
	public:
		std::shared_ptr<T_Asset> store(const std::string& name, const T_Descriptor& description, T_RawAsset rawAsset);

	public:
		static GenerativeAssetManager& instance();

	private:
		std::shared_ptr<T_Asset> from_raw(const std::string& name, T_RawAsset raw, const T_Descriptor& description);
	};

	template<typename T_Descriptor, typename T_RawAsset, typename T_Asset>
	std::shared_ptr<T_Asset> GenerativeAssetManager<T_Descriptor, T_RawAsset, T_Asset>
		::store(const std::string& name, const T_Descriptor& description, T_RawAsset raw)
	{
		const auto itr{ AssetManager<T_Descriptor, T_Asset>::m_store.find(name) };
		if (itr != AssetManager<T_Descriptor, T_Asset>::m_store.end())
			return itr->second.second;

		std::shared_ptr<T_Asset> asset{ from_raw(name, raw, description) };

		if (asset && asset->is_loaded())
		{
			const auto [idx, success] = AssetManager<T_Descriptor, T_Asset>::m_store.try_emplace(name, std::pair(description, asset));

			if (success)
				return idx->second.second;
		}

		return nullptr;
	}

	template<typename T_Descriptor, typename T_RawAsset, typename T_Asset>
	GenerativeAssetManager<T_Descriptor, T_RawAsset, T_Asset>& GenerativeAssetManager<T_Descriptor, T_RawAsset, T_Asset>::instance()
	{
		static GenerativeAssetManager<T_Descriptor, T_RawAsset, T_Asset> s_instance;
		return s_instance;
	}
}

