#pragma once
#include "Resource.h"
class Geometry : public BaseResource<Geometry>
{
	friend class BaseResource<Geometry>;
public:
	Geometry(size_t hash, const std::wstring& szFilePath);
	Geometry(const Geometry&) = delete;
	Geometry& operator=(const Geometry&) = delete;
	Geometry(Geometry&&) = delete;
	Geometry& operator=(Geometry&&) = delete;

public:
	const std::map<UINT, std::vector<Vertex_PTNTB>>& GetVertices() { return m_verticesByMaterial; }
	const std::map<UINT, std::vector<UINT>>& GetIndices() { return m_indicesByMaterial; }
	const std::vector<std::vector<Vector3>>& GetPoints() { return m_pointsByMeshs; }
	std::map<UINT, MTL_TEXTURES>& GetTextures() { return m_texturesByMaterial; }
private:
	void ProcessMesh(const aiScene* scene, const aiMesh* mesh, std::map<UINT, std::unordered_map<Vertex_PTNTB, UINT, Vertex_PTNTB_Hash>>& verticesIdentical);
	void ProcessNode(const aiScene* scene, const aiNode* node, std::map<UINT, std::unordered_map<Vertex_PTNTB, UINT, Vertex_PTNTB_Hash>>& verticesIdentical);

private:
	std::map<UINT, std::vector<Vertex_PTNTB>> m_verticesByMaterial;
	std::map<UINT, std::vector<UINT>> m_indicesByMaterial;
	std::vector<std::vector<Vector3>> m_pointsByMeshs;
	std::map<UINT, MTL_TEXTURES> m_texturesByMaterial;
};

inline Geometry::Geometry(size_t hash, const std::wstring& szFilePath)
{
	SetHash(hash);
	SetFilePath(szFilePath);
	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);
	std::string szFullPath = _towm(szFilePath);
	unsigned int readFlag =
		aiProcess_Triangulate | 
		aiProcess_SortByPType | 
		//aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals | 
		aiProcess_CalcTangentSpace | 
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_LimitBoneWeights |
		aiProcess_ConvertToLeftHanded;
	const aiScene* scene = importer.ReadFile(szFullPath, readFlag);
	_ASEERTION_NULCHK(scene, "aiScene nullptr");
	std::map<UINT, std::unordered_map<Vertex_PTNTB, UINT, Vertex_PTNTB_Hash>> verticesIdentical;
	ProcessNode(scene, scene->mRootNode, verticesIdentical);
}


inline void Geometry::ProcessMesh(const aiScene* scene, const aiMesh* mesh, std::map<UINT, std::unordered_map<Vertex_PTNTB, UINT, Vertex_PTNTB_Hash>>& verticesIdentical)
{
	if (mesh->mNumVertices <= 0) return;
	std::vector<unsigned int> idxs(mesh->mNumVertices);
	std::vector<Vector3> points;
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		float vx, vy, vz, uvx, uvy, nx, ny, nz, tx, ty, tz, bx, by, bz;
		vx = vy = vz = uvx = uvy = nx = ny = nz = tx = ty = tz = bx = by = bz = 0.0f;
		vx = mesh->mVertices[i].x;
		vy = mesh->mVertices[i].y;
		vz = mesh->mVertices[i].z;

		if (mesh->HasTextureCoords(0))
		{
			uvx = (float)mesh->mTextureCoords[0][i].x;
			uvy = (float)mesh->mTextureCoords[0][i].y;
		}

		if (mesh->HasNormals())
		{
			nx = mesh->mNormals[i].x;
			ny = mesh->mNormals[i].y;
			nz = mesh->mNormals[i].z;
		}

		if (mesh->HasTangentsAndBitangents())
		{
			tx = mesh->mTangents[i].x;
			ty = mesh->mTangents[i].y;
			tz = mesh->mTangents[i].z;
			bx = mesh->mBitangents[i].x;
			by = mesh->mBitangents[i].y;
			bz = mesh->mBitangents[i].z;
		}
		Vertex_PTNTB vertex{ { vx, vy, vz }, { uvx, uvy }, { nx, ny, nz }, {tx, ty, tz}, {bx, by, bz } };
		auto it = verticesIdentical[mesh->mMaterialIndex].find(vertex);
		if (it == verticesIdentical[mesh->mMaterialIndex].end())
		{
			idxs[i] = (UINT)m_verticesByMaterial[mesh->mMaterialIndex].size();
			verticesIdentical[mesh->mMaterialIndex][vertex] = (UINT)m_verticesByMaterial[mesh->mMaterialIndex].size();
			m_verticesByMaterial[mesh->mMaterialIndex].push_back(vertex);
			points.push_back({ vx, vy, vz });
		}
		else
		{
			idxs[i] = it->second;
		}
	}

	m_pointsByMeshs.push_back(points);

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
			m_indicesByMaterial[mesh->mMaterialIndex].push_back(idxs[face.mIndices[j]]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		m_texturesByMaterial[mesh->mMaterialIndex].szMatName = material->GetName().C_Str();
		//aiTextureType_GLTF_METALLIC_ROUGHNESS is last(27)
		for (UINT type = 0; type <= aiTextureType_GLTF_METALLIC_ROUGHNESS; type++)
		{
			aiTextureType texType = (aiTextureType)type;
			for (UINT idx = 0; idx < material->GetTextureCount(texType); idx++)
			{
				aiString texPath;
				if (material->GetTexture(texType, idx, &texPath) == aiReturn_SUCCESS)
				{
					const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texPath.C_Str());
					if (embeddedTexture)
					{
						/*
						* mHeight가 0이면 데이터가 압축된 상태(png, jpg 등)임을 의미
						* mHeight == 0일 때: mWidth = Byte Size (파일 용량)
						* mHeight > 0일 때 : mWidth = Pixel Width(이미지 너비)
						*/
						if (embeddedTexture->mHeight == 0)
						{
							ScratchImage srcatchImage;
							LoadFromWICMemory(
								reinterpret_cast<const uint8_t*>(embeddedTexture->pcData), 
								static_cast<size_t>(embeddedTexture->mWidth), 
								DirectX::WIC_FLAGS_NONE,
								nullptr,
								srcatchImage);
							m_texturesByMaterial[mesh->mMaterialIndex].type_textures_image[texType].push_back(std::move(srcatchImage));
						}
					}
					//외부경로저장
					m_texturesByMaterial[mesh->mMaterialIndex].type_textures_path[texType].push_back(texPath.C_Str());
				}
			}
		}
	}
}

inline void Geometry::ProcessNode(const aiScene* scene, const aiNode* node, std::map<UINT, std::unordered_map<Vertex_PTNTB, UINT, Vertex_PTNTB_Hash>>& verticesIdentical)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		ProcessMesh(scene, scene->mMeshes[node->mMeshes[i]], verticesIdentical);
	}


	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(scene, node->mChildren[i], verticesIdentical);
	}
}

