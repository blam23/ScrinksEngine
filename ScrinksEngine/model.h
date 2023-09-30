#pragma once

#include "helpers.h"
#include "asset.h"
#include "mesh.h"

#include <string>

namespace scrinks::render
{
	class Model;
	using ModelDescription = std::string;
	using ModelManager = core::AssetManager<ModelDescription, Model>;

	class Model : public core::Asset
	{
	public:
		Model(Badge<ModelManager>, const std::string& name, std::vector<Mesh>&& meshes);

		void draw(std::shared_ptr<Shader> shader) const;

	private:
		std::vector<Mesh> m_meshes;
	};
}
