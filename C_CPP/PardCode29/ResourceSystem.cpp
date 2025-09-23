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
				tinyobj::index_t index = shape.mesh.indices[offset_shape + v];

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
					tx = attributes.texcoords[index.texcoord_index * 2 + 0];
					ty = 1.0f - attributes.texcoords[index.texcoord_index * 2 + 1];
				}

				if (index.normal_index >= 0)
				{
					nx = attributes.normals[index.normal_index * 3 + 0];
					ny = attributes.normals[index.normal_index * 3 + 1];
					nz = attributes.normals[index.normal_index * 3 + 2];
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

template<>
Mesh<Vertex_PTN>* ResourceSystem::CreateResourceFromFile<Mesh<Vertex_PTN>>(const std::wstring& szFilePath)
{
	size_t hash = Hasing_wstring(szFilePath);
	if (m_Resources.find(hash) != m_Resources.end()) return static_cast<Mesh<Vertex_PTN>*>(m_Resources[hash]);

	std::string szFullPath = _towm(szFilePath);
	std::string szMtlBasePath = szFullPath.substr(0, szFullPath.find_last_of('/'));
	std::string extension = szFullPath.substr(szFullPath.find_last_of('.'));

	Mesh<Vertex_PTN>* newResource = nullptr;
	std::vector<std::vector<Vertex_PTN>> verticesByMaterial;
	std::vector<std::vector<UINT>> indicesByMaterial;
	std::vector<std::vector<Vector3>> pointsByMaterial;
	if (extension == ".obj")
	{
		ParseObj<Vertex_PTN, Vertex_PTN_Hash>(szFullPath, szMtlBasePath, verticesByMaterial, indicesByMaterial, pointsByMaterial);
		std::vector<Vertex_PTN> vertices;
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
			psum += verticesByMaterial[iidx].size();
		}

		newResource = new Mesh<Vertex_PTN>(hash, szFilePath, std::move(pointsByMaterial), std::move(vertices), std::move(countsVertices), std::move(indices), std::move(countsIndices));
		_ASEERTION_NULCHK(newResource, typeid(Mesh<Vertex_PTN>).name());
		m_Resources[hash] = newResource;
		return newResource;
	}
	else if (extension == ".fbx")
	{

	}
	return newResource;
}

//TBN구성에 사용할 tangent(접벡터), binormal(종벡터)를 계산한다
/*
		MatE = deltaUV * MatTB
		MatTB = deltaUV^-1 * MatE
		* 역행렬
		* AdjMat(A*) = C(A)^T , 여인수행렬의 전치를 의미, 수반행렬
		* InvMat = AdjMat * 1.0f/detA
		deltaUV^-1 = 1/(ad-bc) * AdjMat

		deltaUV = du0 dv0 (a, b)
				  du1 dv1 (c, d)

		adjMat = d -b
				-c  a

		invMat = denominator * adjMat
*/
void ComputeTangentBinormal(const std::vector<UINT>& indicies, std::vector<Vertex_PTNTB>& vertices)
{
	//메쉬(세 정점)을 기준으로 tangent, binormal의 누적을 계산한다
	for (UINT idx = 0; idx < indicies.size(); idx += 3)
	{
		UINT i0 = indicies[idx];
		UINT i1 = indicies[idx + 1];
		UINT i2 = indicies[idx + 2];
		Vector3 e0 = vertices[i1].pos0 - vertices[i0].pos0;	
		Vector3 e1 = vertices[i2].pos0 - vertices[i0].pos0;
		Vector2 uv_e0 = vertices[i1].tex0 - vertices[i0].tex0;	//a, b
		Vector2 uv_e1 = vertices[i2].tex0 - vertices[i0].tex0;	//c, d
		float det = uv_e0.GetX() * uv_e1.GetY() - uv_e0.GetY() * uv_e1.GetX();	//ad - bc
		if (std::fabs(det) <= _EPSILON) continue;
		det = 1.0f / det;
		float tx, ty, tz;
		tx = (uv_e1.GetY() * e0.GetX() - uv_e0.GetY() * e1.GetX()) * det;
		ty = (uv_e1.GetY() * e0.GetY() - uv_e0.GetY() * e1.GetY()) * det;
		tz = (uv_e1.GetY() * e0.GetZ() - uv_e0.GetY() * e1.GetZ()) * det;

		Vector3 tangent(tx, ty, tz);
		vertices[i0].tangent0 += tangent;
		vertices[i1].tangent0 += tangent;
		vertices[i2].tangent0 += tangent;

		//float bx, by, bz;
		//bx = (-uv_e1.GetX() * e0.GetX() + uv_e0.GetX() * e1.GetX()) * det;
		//by = (-uv_e1.GetX() * e0.GetY() + uv_e0.GetX() * e1.GetY()) * det;
		//bz = (-uv_e1.GetX() * e0.GetZ() + uv_e0.GetX() * e1.GetZ()) * det;
		//Vector3 binormal(bx, by, bz);
		////binormal = binormal.Normalize();
		//vertices[i0].binormal0 += binormal;
		//vertices[i1].binormal0 += binormal;
		//vertices[i2].binormal0 += binormal;
	}

	//그람슈미트직교화를이용
	for (auto& vertex : vertices)
	{
		vertex.normal0 = vertex.normal0.Normalize();
		vertex.tangent0 = (vertex.tangent0 - (vertex.normal0 * vertex.normal0.DotProduct(vertex.tangent0))).Normalize();
		vertex.binormal0 = vertex.normal0.CrossProduct(vertex.tangent0);
	}
}

template<>
Mesh<Vertex_PTNTB>* ResourceSystem::CreateResourceFromFile<Mesh<Vertex_PTNTB>>(const std::wstring& szFilePath)
{
	size_t hash = Hasing_wstring(szFilePath);
	if (m_Resources.find(hash) != m_Resources.end()) return static_cast<Mesh<Vertex_PTNTB>*>(m_Resources[hash]);

	std::string szFullPath = _towm(szFilePath);
	std::string szMtlBasePath = szFullPath.substr(0, szFullPath.find_last_of('/'));
	std::string extension = szFullPath.substr(szFullPath.find_last_of('.'));

	Mesh<Vertex_PTNTB>* newResource = nullptr;
	std::vector<std::vector<Vertex_PTNTB>> verticesByMaterial;
	std::vector<std::vector<UINT>> indicesByMaterial;
	std::vector<std::vector<Vector3>> pointsByMaterial;
	if (extension == ".obj")
	{
		ParseObj<Vertex_PTNTB, Vertex_PTNTB_Hash>(szFullPath, szMtlBasePath, verticesByMaterial, indicesByMaterial, pointsByMaterial);
		std::vector<Vertex_PTNTB> vertices;
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
			psum += verticesByMaterial[iidx].size();
		}

		ComputeTangentBinormal(indices, vertices);

		newResource = new Mesh<Vertex_PTNTB>(hash, szFilePath, std::move(pointsByMaterial), std::move(vertices), std::move(countsVertices), std::move(indices), std::move(countsIndices));
		_ASEERTION_NULCHK(newResource, typeid(Mesh<Vertex_PTNTB>).name());
		m_Resources[hash] = newResource;
		return newResource;
	}
	else if (extension == ".fbx")
	{

	}
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
		pMaterial->SetTexturePath({ E_Textures::Diffuse, iter.diffuse_texname });
		rets.push_back(pMaterial);
	}
	return rets;
}