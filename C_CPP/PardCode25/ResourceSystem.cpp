#define TINYOBJLOADER_IMPLEMENTATION
#include "ResourceSystem.h"
#include "Resource.h"
#include "Mesh.h"
#include "Material.h"
#include <tiny_obj_loader.h>
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
}

void ResourceSystem::Init()
{

}

void ResourceSystem::Release()
{
	for (auto iter = m_Resources.begin(); iter != m_Resources.end();)
	{
		delete iter->second;
		iter = m_Resources.erase(iter);
	}
}

template<>
Mesh* ResourceSystem::CreateResourceFromFile<Mesh>(const std::wstring& szFilePath)
{
	size_t hash = HashingFile(szFilePath);
	if (m_Resources.find(hash) != m_Resources.end()) return static_cast<Mesh*>(m_Resources[hash]);
	Mesh* newResource = nullptr;
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning;
	std::string error;
	std::string szFullPath = _towm(szFilePath);
	std::string szMtlBasePath = szFullPath.substr(0, szFullPath.find_last_of('/'));
	std::string extension = szFullPath.substr(szFullPath.find_last_of('.'));
	if (extension == ".obj")
	{
		_ASEERTION_NULCHK(tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, szFullPath.c_str(), szMtlBasePath.c_str()), "MeshCreateFailed");
		_ASEERTION_NULCHK(error.empty(), "MeshCreateFailed");
		_ASEERTION_NULCHK(shapes.size() >= 1, "MeshCreateFailed");
		std::vector<std::vector<Vertex_PTN>> vertices;
		std::vector<std::vector<UINT>> indices;
		std::vector<std::vector<Vector3>> points;
		vertices.resize(std::max(materials.size(), (size_t)1));
		indices.resize(std::max(materials.size(), (size_t)1));
		points.resize(std::max(materials.size(), (size_t)1));

		UINT offset_indices = 0;
		for (UINT sidx = 0; sidx < shapes.size(); sidx++)
		{
			UINT offset_shape = 0;
			for (UINT fidx = 0; fidx < shapes[sidx].mesh.num_face_vertices.size(); fidx++)
			{
				UINT midx = std::max(shapes[sidx].mesh.material_ids[fidx], 0);
				unsigned char face_vertices = shapes[sidx].mesh.num_face_vertices[fidx];
				for (unsigned char v = 0; v < face_vertices; v++)
				{
					//특정 쉐이프(shape, 모델, sidx) 의 메쉬에서 인덱스값을 기반으로 정점정보의 절대값(attrib)을 가져온다
					tinyobj::index_t index = shapes[sidx].mesh.indices[offset_shape + v];
					float vx, vy, vz, tx, ty, nx, ny, nz;
					vx = vy = vz = tx = ty = nx = ny = nz = 0.0f;
					vx = attributes.vertices[index.vertex_index * 3 + 0];
					vy = attributes.vertices[index.vertex_index * 3 + 1];
					vz = attributes.vertices[index.vertex_index * 3 + 2];
					if (index.texcoord_index >= 0)
					{
						// V 텍스처 좌표를 왼손 시스템으로 반전시킵니다.
						//texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
						//tx = 1.0f - tx;
						//ty = 1.0f - ty;
						tx = 1.0f - attributes.texcoords[index.texcoord_index * 2 + 0];
						ty = 1.0f - attributes.texcoords[index.texcoord_index * 2 + 1];
					}

					if (index.normal_index >= 0)
					{
						nx = attributes.normals[index.normal_index * 3 + 0];
						ny = attributes.normals[index.normal_index * 3 + 1];
						nz = attributes.normals[index.normal_index * 3 + 2];
					}
					points[midx].push_back({ vx, vy, vz });
					vertices[midx].push_back({ {vx, vy, vz}, {tx, ty}, {nx, ny, nz} });
					indices[midx].push_back(offset_indices + (UINT)v);
				}
				offset_shape += face_vertices;
				offset_indices += face_vertices;
			}
		}
		
		//객체생성후 컨테이너에 등록
		newResource = new Mesh(hash, szFilePath, &points, &vertices, &indices);
		_ASEERTION_NULCHK(newResource, typeid(Mesh).name());
		m_Resources[hash] = newResource;
	}
	else if (extension == ".fbx")
	{

	}
	return newResource;
}

template<>
Material* ResourceSystem::CreateResourceFromFile<Material>(const std::wstring& szFilePath)
{
	size_t hash = HashingFile(szFilePath);
	if (m_Resources.find(hash) != m_Resources.end()) return static_cast<Material*>(m_Resources[hash]);
	//객체생성후 컨테이너에 등록
	Material* newResource = nullptr; 

	std::string szFullPath = _towm(szFilePath);
	auto iter = szFullPath.find_last_of('.');
	std::map<std::string, int> mtlidxs;
	std::vector<tinyobj::material_t> materials;
	std::ifstream matIStream(szFullPath.c_str());
	std::string warning;
	std::string error;

	if (iter != std::string::npos)
	{
		tinyobj::LoadMtl(&mtlidxs, &materials, &matIStream, &warning, &error);
		for (const auto& iter : materials)
		{

		}
	}
	else
	{
		
	}
	newResource = new Material(hash, szFilePath);
	_ASEERTION_NULCHK(newResource, typeid(Material).name());
	m_Resources[hash] = newResource;
	return newResource;
}