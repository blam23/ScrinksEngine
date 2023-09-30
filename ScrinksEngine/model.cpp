#include "model.h"

#include <iostream>

#undef min
#undef max
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace scrinks::render;

scrinks::render::Model::Model(Badge<ModelManager>, const std::string& name, std::vector<Mesh>&& meshes)
	: Asset{ name, 0 }
	, m_meshes{ std::move(meshes) }
{
	m_loaded = true;
}

void scrinks::render::Model::draw(std::shared_ptr<Shader> shader) const
{
	for (const auto& mesh : m_meshes)
		mesh.draw(shader);
}

void load_textures_from_material
(
	TextureVector& textures,
	aiMaterial* mat,
	aiTextureType type,
	const std::string& relativePath
)
{
	for (std::uint32_t i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		const auto& fullPath{ relativePath + "/" + str.C_Str() };
		auto texture{ TextureManager::instance().load_and_store(fullPath, fullPath) };

		if (texture)
			textures.push_back(texture);
		else
		{
			std::cerr << "Error loading model, unable to load texture: '" << fullPath << "'." << std::endl;
		}
	}
}

void convert_mesh(std::vector<Mesh>& meshes, aiMesh* mesh, const aiScene* scene, const std::string& relativePath)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	TextureVector textures;

	for (std::uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex v
		{
			{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z },
			{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z },
			mesh->mTextureCoords[0] ?
				glm::vec2{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y } :
				glm::vec2{ 0.0f, 0.0f }
		};

		vertices.push_back(v);
	}

	for (std::uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		for (std::uint32_t j = 0; j < mesh->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* mat{ scene->mMaterials[mesh->mMaterialIndex] };
		load_textures_from_material(textures, mat, aiTextureType_DIFFUSE, relativePath);
		load_textures_from_material(textures, mat, aiTextureType_SPECULAR, relativePath);
	}

	meshes.emplace_back(std::move(vertices), std::move(indices), std::move(textures));
}

void collect(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, const std::string& relativePath)
{
	for (std::uint32_t i = 0; i < node->mNumMeshes; i++)
		convert_mesh(meshes, scene->mMeshes[node->mMeshes[i]], scene, relativePath);

	// recursively collect child meshes
	for (std::uint32_t i = 0; i < node->mNumChildren; i++)
		collect(node->mChildren[i], scene, meshes, relativePath);
}

template <>
std::shared_ptr<Model> ModelManager::load(const std::string& name, const ModelDescription& path)
{
	Assimp::Importer importer;
	const aiScene* scene{ importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs) };

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Error importing model: " << importer.GetErrorString() << std::endl;
		return {};
	}

	std::string relativePath{ path.substr(0, path.find_last_of('/')) };
	
	std::vector<Mesh> meshes;
	collect(scene->mRootNode, scene, meshes, relativePath);

	return std::make_shared<Model>(Badge<ModelManager>{}, name, std::move(meshes));
}
