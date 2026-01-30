#pragma once
#include "BaseSystem.h"
#include "Mesh.h"

//class Material;
class Resource;
class ResourceSystem : public BaseSystem<ResourceSystem>
{
	friend class BaseSystem<ResourceSystem>;	//CRTP패턴을 위한 friend선언
private:
	ResourceSystem();
	ResourceSystem(const ResourceSystem&) = delete;
	ResourceSystem& operator=(const ResourceSystem&) = delete;
	ResourceSystem(ResourceSystem&&) = delete;
	ResourceSystem& operator=(ResourceSystem&&) = delete;
	
public:
	~ResourceSystem();
	void Init();
	template<typename T, typename... Types>
	T* CreateResourceFromFile(const std::wstring& szFilePath, Types&&... args);

	template<typename T>
	Mesh<T>* CreateMeshFromFile(const std::wstring& szFilePath, std::map<UINT, MTL_TEXTURES>& texturesByMaterial);

	/*template<typename T>
	Mesh<T>* CreateMeshFromGeometry(const std::wstring szName, std::vector<std::vector<Vector3>>&& points, std::vector<T>&& vertices, std::vector<UINT>&& indices);*/
	/*
	* //not use tinyobj, use assimp
	template<typename T, typename... Types>
	std::vector<T*> CreateResourcesFromFile(const std::wstring& szFilePath, Types&&... args);
	*/
	template<typename T>
	T* GetResource(size_t hash);

private:
	template<typename T, typename T_HASH>
	void ProcessMesh(aiMesh* mesh, const aiScene* scene, std::map<UINT, std::vector<T>>& vertices, std::map<UINT, std::vector<UINT>>& indices, std::map<UINT, std::vector<Vector3>>& points, std::map<UINT, std::unordered_map<T, UINT, T_HASH>>& vertexMaps, std::map<UINT, MTL_TEXTURES>& textures);

	template<typename T, typename T_HASH>
	void ProcessNode(aiNode* node, const aiScene* scene, std::map<UINT, std::vector<T>>& vertices, std::map<UINT, std::vector<UINT>>& indices, std::map<UINT, std::vector<Vector3>>& points, std::map<UINT, std::unordered_map<T, UINT, T_HASH>>& vertexMaps, std::map<UINT, MTL_TEXTURES>& textures);
	
	template<typename T, typename T_HASH>
	void ParseScene(const std::string& szFullPath, std::map<UINT, std::vector<T>>& vertices, std::map<UINT, std::vector<UINT>>& indices, std::map<UINT, std::vector<Vector3>>& points, std::map<UINT, std::unordered_map<T, UINT, T_HASH>>& vertexMaps, std::map<UINT, MTL_TEXTURES>& textures);

private:
	std::unordered_map<size_t, Resource*> m_Resources;
};
#define _ResourceSystem ResourceSystem::GetInstance()


template<typename T, typename... Types>
T* ResourceSystem::CreateResourceFromFile(const std::wstring& szFilePath, Types&&... args)
{
	size_t hash = Hasing_wstring(szFilePath);
	if (m_Resources.find(hash) != m_Resources.end()) return static_cast<T*>(m_Resources[hash]);
	//객체생성후 컨테이너에 등록
	T* newResource = new T(hash, szFilePath, std::forward<Types>(args)...);
	_ASEERTION_NULCHK(newResource, typeid(T).name());
	m_Resources[hash] = newResource;
	return newResource;
}

template<typename T>
T* ResourceSystem::GetResource(size_t hash)
{
	_ASEERTION_NULCHK(m_Resources.find(hash) != m_Resources.end(), "hash is null");
	return static_cast<T*>(m_Resources[hash]);
}

template<typename T, typename T_HASH>
inline void ResourceSystem::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::map<UINT, std::vector<T>>& vertices, std::map<UINT, std::vector<UINT>>& indices, std::map<UINT, std::vector<Vector3>>& points, std::map<UINT, std::unordered_map<T, UINT, T_HASH>>& vertexMaps, std::map<UINT, MTL_TEXTURES>& textures)
{
	if (mesh->mNumVertices <= 0) return;
	std::vector<unsigned int> idxs(mesh->mNumVertices);
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		float vx, vy, vz, tx, ty, nx, ny, nz;
		vx = vy = vz = tx = ty = nx = ny = nz = 0.0f;
		vx = mesh->mVertices[i].x;
		vy = mesh->mVertices[i].y;
		vz = mesh->mVertices[i].z;

		if (mesh->HasTextureCoords(0))
		{
			tx = (float)mesh->mTextureCoords[0][i].x;
			ty = (float)mesh->mTextureCoords[0][i].y;
		}

		if (mesh->HasNormals())
		{
			nx = mesh->mNormals[i].x;
			ny = mesh->mNormals[i].y;
			nz = mesh->mNormals[i].z;
		}

		T vertex{ {vx, vy, vz}, {tx, ty}, {nx, ny, nz} };
		auto it = vertexMaps[mesh->mMaterialIndex].find(vertex);
		if (it == vertexMaps[mesh->mMaterialIndex].end())
		{
			idxs[i] = (UINT)vertices[mesh->mMaterialIndex].size();
			vertexMaps[mesh->mMaterialIndex][vertex] = (UINT)vertices[mesh->mMaterialIndex].size();
			vertices[mesh->mMaterialIndex].push_back(vertex);
			points[mesh->mMaterialIndex].push_back({ vx, vy, vz });
		}
		else
		{
			idxs[i] = it->second;
		}
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
			indices[mesh->mMaterialIndex].push_back(idxs[face.mIndices[j]]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		//aiTextureType_GLTF_METALLIC_ROUGHNESS is last(27)
		for (UINT type = 0; type <= aiTextureType_GLTF_METALLIC_ROUGHNESS; type++)
		{
			aiTextureType texType = (aiTextureType)type;
			for (UINT idx = 0; idx < material->GetTextureCount(texType); idx++)
			{
				aiString texPath;
				if (material->GetTexture(texType, idx, &texPath) == aiReturn_SUCCESS)
					textures[mesh->mMaterialIndex][texType].push_back(texPath.C_Str());
			}
		}
	}
}

template<typename T, typename T_HASH>
inline void ResourceSystem::ProcessNode(aiNode* node, const aiScene* scene, std::map<UINT, std::vector<T>>& vertices, std::map<UINT, std::vector<UINT>>& indices, std::map<UINT, std::vector<Vector3>>& points, std::map<UINT, std::unordered_map<T, UINT, T_HASH>>& vertexMaps, std::map<UINT, MTL_TEXTURES>& textures)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
		ProcessMesh<T, T_HASH>(scene->mMeshes[node->mMeshes[i]], scene, vertices, indices, points, vertexMaps, textures);

	for (UINT i = 0; i < node->mNumChildren; i++)
		ProcessNode<T, T_HASH>(node->mChildren[i], scene, vertices, indices, points, vertexMaps, textures);
}

template<typename T, typename T_HASH>
inline void ResourceSystem::ParseScene(const std::string& szFullPath, std::map<UINT, std::vector<T>>& vertices, std::map<UINT, std::vector<UINT>>& indices, std::map<UINT, std::vector<Vector3>>& points, std::map<UINT, std::unordered_map<T, UINT, T_HASH>>& vertexMaps, std::map<UINT, MTL_TEXTURES>& textures)
{
	Assimp::Importer importer;
	unsigned int readFlag = aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_ConvertToLeftHanded;
	const aiScene* scene = importer.ReadFile(szFullPath, readFlag);
	_ASEERTION_NULCHK(scene, "aiScene nullptr");
	ProcessNode<T, T_HASH>(scene->mRootNode, scene, vertices, indices, points, vertexMaps, textures);
}

extern std::wstring g_szMesh_PTN;
extern std::wstring g_szMesh_PTNTB;

template<typename T>
inline Mesh<T>* ResourceSystem::CreateMeshFromFile(const std::wstring& szFilePath, std::map<UINT, MTL_TEXTURES>& texturesByMaterial)
{
	std::wstring pathTag = std::is_same_v<T, Vertex_PTN> ? g_szMesh_PTN : g_szMesh_PTNTB;
	std::wstring szName = szFilePath + pathTag;
	size_t hash = Hasing_wstring(szName);
	if (m_Resources.find(hash) != m_Resources.end()) 
		return static_cast<Mesh<T>*>(m_Resources[hash]);
	
	std::string szFullPath = _towm(szFilePath);
	//std::string szMtlBasePath = szFullPath.substr(0, szFullPath.find_last_of('/'));
	//std::string extension = szFullPath.substr(szFullPath.find_last_of('.'));

	std::map<UINT, std::vector<T>> verticesByMaterial;
	std::map<UINT, std::vector<UINT>> indicesByMaterial;
	std::map<UINT, std::vector<Vector3>> pointsByMaterial;
	using HASH_TYPE = std::conditional_t<std::is_same_v<T, Vertex_PTN>, Vertex_PTN_Hash, Vertex_PTNTB_Hash>;
	std::map<UINT, std::unordered_map<T, UINT, HASH_TYPE>> vertexMaps;
	ParseScene<T, HASH_TYPE>(szFullPath, verticesByMaterial, indicesByMaterial, pointsByMaterial, vertexMaps, texturesByMaterial);

	//Mesh로 정렬해 넣기위해 순서대로 Material에 따른 인덱싱을 재계산
	std::vector<T> vertices;
	std::vector<UINT> indices;
	std::vector<std::vector<Vector3>> points;
	std::vector<RenderCounts> countsVertices;
	std::vector<RenderCounts> countsIndices;
	UINT psum = 0, idx = 0;
	for (const auto& iter : verticesByMaterial)
	{
		countsVertices.push_back({ (UINT)iter.second.size(), idx });
		idx = (UINT)iter.second.size();
		vertices.insert(vertices.end(), iter.second.begin(), iter.second.end());
	}

	idx = 0;
	for (const auto& iter : indicesByMaterial)
	{
		countsIndices.push_back({ (UINT)iter.second.size(), idx });
		idx += (UINT)iter.second.size();
		for (auto& vidx : iter.second)
			indices.push_back(psum + vidx);
		psum += (UINT)verticesByMaterial[iter.first].size();
	}

	for (const auto& iter : pointsByMaterial)
		points.push_back(iter.second);

	if constexpr (std::is_same_v<T, Vertex_PTNTB>)
		ComputeTangentBinormal(indices, vertices);

	return _ResourceSystem.CreateResourceFromFile<Mesh<T>>(szName, std::move(points), std::move(vertices), std::move(countsVertices), std::move(indices), std::move(countsIndices));
}

//template<typename T>
//inline Mesh<T>* ResourceSystem::CreateMeshFromGeometry(const std::wstring szName, std::vector<std::vector<Vector3>>&& points, std::vector<T>&& vertices, std::vector<UINT>&& indices)
//{
//	using VTYPE = typename T::VertexType;
//	std::wstring pathTag = std::is_same_v<VTYPE, Vertex_PTN> ? g_szMesh_PTN : g_szMesh_PTNTB;
//	size_t hash = Hasing_wstring(szFilePath + pathTag);
//	if (m_Resources.find(hash) != m_Resources.end()) return static_cast<T*>(m_Resources[hash]);
//	Mesh<VTYPE>* newResource = CreateMeshFromFile<VTYPE>(hash, szFilePath, texturesByMaterial);
//	m_Resources[hash] = newResource;
//	return newResource;
//}

/*
* //not use tinyobj, use assimp
template<typename T, typename ...Types>
inline std::vector<T*> ResourceSystem::CreateResourcesFromFile(const std::wstring& szFilePath, Types&& ...args)
{
	return std::vector<T*>();
}

//템플릿특수화
template<>
Mesh<Vertex_PTN>* ResourceSystem::CreateResourceFromFile<Mesh<Vertex_PTN>>(const std::wstring& szFilePath, std::map<UINT, MTL_TEXTURES>& texturesByMaterial);

template<>
Mesh<Vertex_PTNTB>* ResourceSystem::CreateResourceFromFile<Mesh<Vertex_PTNTB>>(const std::wstring& szFilePath, std::map<UINT, MTL_TEXTURES>& texturesByMaterial);

template<>
std::vector<Material*> ResourceSystem::CreateResourcesFromFile<Material>(const std::wstring& szFilePath);
*/
