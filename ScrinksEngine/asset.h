#pragma once

#include <memory>
#include <map>
#include <functional>
#include <string>
#include "helpers.h"

namespace scrinks::core
{
	/// <summary>
	/// A base-class for loadable/generatable assets, such as Scripts or Shaders.
	/// </summary>
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

	/// <summary>
	/// For managing loadable assets such as shaders, scripts and models
	/// </summary>
	/// <typeparam name="T_Descriptor">Data needed to load the asset, such as paths to the vertex and fragment shaders</typeparam>
	/// <typeparam name="T_Asset">The type of asset to be managed, such as Shader or Model</typeparam>
	template <typename T_Descriptor, typename T_Asset>
	class AssetManager
	{
	public:
		static std::shared_ptr<T_Asset> load_and_store(const std::string& name, const T_Descriptor& description);
		static std::shared_ptr<T_Asset> reload_and_store(const std::string& name, const T_Descriptor& description);
		static std::shared_ptr<T_Asset> reload(const std::string& name);
		static std::shared_ptr<T_Asset> get(const std::string& name);
		static void mark_for_removal(const std::string& name);
		static void reload_all();
		static void for_each(std::function<void(const std::string& name, std::shared_ptr<T_Asset>)> func);
		static void for_each_name(std::function<void(const std::string& name)> func);

	protected:
		static std::shared_ptr<T_Asset> load(const std::string& name, const T_Descriptor& description);
		static inline std::map<std::string, std::pair<T_Descriptor, std::shared_ptr<T_Asset>>> m_store{};
	};

	template<typename T_Descriptor, typename T_Asset>
	std::shared_ptr<T_Asset> AssetManager<T_Descriptor, T_Asset>::load_and_store(
		const std::string& name,
		const T_Descriptor& description
	)
	{
		// If the asset already is loaded, return it
		const auto itr{ m_store.find(name) };
		if (itr != m_store.end())
			return itr->second.second;

		// Try and load the asset
		const auto asset{ load(name, description) };

		// Only emplace if the asset was loaded successfully
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

	/// <summary>
	/// For managing generated assets that you want to be sharable by name, such as buffers
	/// </summary>
	/// <typeparam name="T_Descriptor">Data needed to create/recreate the asset, such as buffer size and format.</typeparam>
	/// <typeparam name="T_RawAsset">The raw asset identifier, for example the GLuint of the VBO</typeparam>
	/// <typeparam name="T_Asset">The type of asset to be managed</typeparam>
	template <typename T_Descriptor, typename T_RawAsset, typename T_Asset>
	class GeneratedAssetManager : public AssetManager<T_Descriptor, T_Asset>
	{
	public:
		static std::shared_ptr<T_Asset> store(const std::string& name, const T_Descriptor& description, T_RawAsset rawAsset);

	private:
		static std::shared_ptr<T_Asset> from_raw(const std::string& name, T_RawAsset raw, const T_Descriptor& description);
	};

	template<typename T_Descriptor, typename T_RawAsset, typename T_Asset>
	std::shared_ptr<T_Asset> GeneratedAssetManager<T_Descriptor, T_RawAsset, T_Asset>
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
}

