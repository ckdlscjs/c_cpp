#include "ResourceSystem.h"
#include "Resource.h"
#include "Material.h"
//#include "Mesh.h" //템플릿 클래스화로 인해 h에포함
 
//// FileSystem
//#if __cplusplus <= 201402L 
//#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
//#include <experimental/filesystem>
//#endif
//
//#if __cplusplus >= 201703L
//#include <filesystem>
//#endif

ResourceSystem::ResourceSystem()
{
}

ResourceSystem::~ResourceSystem()
{
	for (auto iter = m_Resources.begin(); iter != m_Resources.end();)
	{
		delete iter->second;
		iter = m_Resources.erase(iter);
	}
}
void ResourceSystem::Init()
{

}



#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
template<typename T, typename T_HASH>
inline void ParseObj(const std::string& szFullPath, const std::string& szMtlBasePath, std::vector<std::vector<T>>& vertices, std::vector<std::vector<UINT>>& indices, std::vector<std::vector<Vector3>>& points)
{
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning;
	std::string error;
	_ASEERTION_NULCHK(tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, szFullPath.c_str(), szMtlBasePath.c_str()), "MeshCreateFailed");
	_ASEERTION_NULCHK(error.empty(), "MeshCreateFailed");
	_ASEERTION_NULCHK(shapes.size() >= 1, "MeshCreateFailed");
	size_t numMaterials = std::max(materials.size(), (size_t)1);
	vertices.resize(numMaterials);
	indices.resize(numMaterials);
	points.resize(numMaterials);

	//해시를 이용한 중복체크
	std::vector<std::unordered_map<T, UINT, T_HASH>> vertexMaps;
	vertexMaps.resize(numMaterials);

	for (const auto& shape : shapes)
	{
		size_t offset_shape = 0;
		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
		{
			UINT midx = std::max(shape.mesh.material_ids[f], 0);
			unsigned char face_vertices = shape.mesh.num_face_vertices[f];

			for (size_t v = 0; v < face_vertices; v++)
			{
				//obj의 일반적인 포맷 리드를 수정, 역순으로읽는다
				tinyobj::index_t index = shape.mesh.indices[offset_shape + (face_vertices - 1) - v];

				float vx, vy, vz, tx, ty, nx, ny, nz;
				vx = vy = vz = tx = ty = nx = ny = nz = 0.0f;
				vx = attributes.vertices[index.vertex_index * 3 + 2];
				vy = attributes.vertices[index.vertex_index * 3 + 1];
				vz = attributes.vertices[index.vertex_index * 3 + 0];
				if (index.texcoord_index >= 0)
				{
					// V 텍스처 좌표를 왼손 시스템으로 반전시킵니다.
					//texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
					//tx = 1.0f - tx;
					//ty = 1.0f - ty;
					tx = attributes.texcoords[index.texcoord_index * 2 + 0];
					ty = 1.0f - attributes.texcoords[index.texcoord_index * 2 + 1];
				}

				if (index.normal_index >= 0)
				{
					nx = attributes.normals[index.normal_index * 3 + 2];
					ny = attributes.normals[index.normal_index * 3 + 1];
					nz = attributes.normals[index.normal_index * 3 + 0];
				}
				T vertex{ {vx, vy, vz}, {tx, ty}, {nx, ny, nz} };
				if (vertexMaps[midx].find(vertex) == vertexMaps[midx].end())
				{
					vertexMaps[midx][vertex] = (UINT)vertices[midx].size();
					vertices[midx].push_back(vertex);
				}

				indices[midx].push_back(vertexMaps[midx][vertex]);
				points[midx].push_back(vertex.pos0);
			}
			offset_shape += face_vertices;
		}
	}
}
template<typename T> 
Mesh<T>* CreateMeshFromFile(size_t hash, const std::wstring& szFilePath)
{
	std::string szFullPath = _towm(szFilePath);
	std::string szMtlBasePath = szFullPath.substr(0, szFullPath.find_last_of('/'));
	std::string extension = szFullPath.substr(szFullPath.find_last_of('.'));

	Mesh<T>* newResource = nullptr;
	std::vector<std::vector<T>> verticesByMaterial;
	std::vector<std::vector<UINT>> indicesByMaterial;
	std::vector<std::vector<Vector3>> pointsByMaterial;
	if (extension == ".obj")
	{
		using HASH_TYPE = std::conditional_t<std::is_same_v<T, Vertex_PTN>, Vertex_PTN_Hash, Vertex_PTNTB_Hash>;
		ParseObj<T, HASH_TYPE>(szFullPath, szMtlBasePath, verticesByMaterial, indicesByMaterial, pointsByMaterial);
		std::vector<T> vertices;
		std::vector<UINT> indices;
		std::vector<RenderCounts> countsVertices;
		std::vector<RenderCounts> countsIndices;

		UINT psum = 0, idx = 0;
		for (UINT vidx = 0; vidx < verticesByMaterial.size(); vidx++)
		{
			countsVertices.push_back({ (UINT)verticesByMaterial[vidx].size(), idx });
			idx = (UINT)verticesByMaterial[vidx].size();
			vertices.insert(vertices.end(), verticesByMaterial[vidx].begin(), verticesByMaterial[vidx].end());
		}

		idx = 0;
		for (UINT iidx = 0; iidx < indicesByMaterial.size(); iidx++)
		{
			countsIndices.push_back({ (UINT)indicesByMaterial[iidx].size(), idx });
			idx += (UINT)indicesByMaterial[iidx].size();
			for (auto& iter : indicesByMaterial[iidx]) indices.push_back(psum + iter);
			psum += (UINT)verticesByMaterial[iidx].size();
		}
		if constexpr(std::is_same_v<T, Vertex_PTNTB>)
			ComputeTangentBinormal(indices, vertices);

		newResource = new Mesh<T>(hash, szFilePath, std::move(pointsByMaterial), std::move(vertices), std::move(countsVertices), std::move(indices), std::move(countsIndices));
		_ASEERTION_NULCHK(newResource, typeid(Mesh<T>).name());
		return newResource;
	}
	else if (extension == ".fbx")
	{
		return newResource;
	}
	return newResource;
}
std::wstring szMesh_PTN = _tomw(typeid(Mesh<Vertex_PTN>).name());
std::wstring szMesh_PTNTB = _tomw(typeid(Mesh<Vertex_PTNTB>).name());
template<>
Mesh<Vertex_PTN>* ResourceSystem::CreateResourceFromFile<Mesh<Vertex_PTN>>(const std::wstring& szFilePath)
{
	size_t hash = Hasing_wstring(szFilePath + szMesh_PTN);
	if (m_Resources.find(hash) != m_Resources.end()) return static_cast<Mesh<Vertex_PTN>*>(m_Resources[hash]);
	Mesh<Vertex_PTN>* newResource = CreateMeshFromFile<Vertex_PTN>(hash, szFilePath);
	m_Resources[hash] = newResource;
	return newResource;
}

template<>
Mesh<Vertex_PTNTB>* ResourceSystem::CreateResourceFromFile<Mesh<Vertex_PTNTB>>(const std::wstring& szFilePath)
{
	size_t hash = Hasing_wstring(szFilePath + szMesh_PTNTB);
	if (m_Resources.find(hash) != m_Resources.end()) return static_cast<Mesh<Vertex_PTNTB>*>(m_Resources[hash]);
	Mesh<Vertex_PTNTB>* newResource = CreateMeshFromFile<Vertex_PTNTB>(hash, szFilePath);
	m_Resources[hash] = newResource;
	return newResource;
}

template<>
std::vector<Material*> ResourceSystem::CreateResourcesFromFile<Material>(const std::wstring& szFilePath)
{
	std::vector<Material*> rets;
	std::string szFullPath = _towm(szFilePath);
	std::string extension = szFullPath.substr(szFullPath.find_last_of('.'));
	_ASEERTION_NULCHK(extension == ".mtl", "not material file");
	std::map<std::string, int> mtlidxs;
	std::vector<tinyobj::material_t> materials;
	std::ifstream matIStream(szFullPath.c_str());
	std::string warning;
	std::string error;
	tinyobj::LoadMtl(&mtlidxs, &materials, &matIStream, &warning, &error);
	for (const auto& iter : materials)
	{
		std::wstring matName = szFilePath + _tomw(iter.name);
		Material* pMaterial = CreateResourceFromFile<Material>(matName);
		pMaterial->SetTexturePath({ E_Texture::Diffuse, iter.diffuse_texname });
		rets.push_back(pMaterial);
	}
	return rets;
}