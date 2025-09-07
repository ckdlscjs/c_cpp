#include "Mesh.h"
//#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh::Mesh(size_t hash, const std::wstring& szFilePath)
{
	SetHash(hash);
	SetFilePath(szFilePath);
	
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning;
	std::string error;
	std::string szFullPath = _towm(szFilePath);
	std::string szMtlBasePath = szFullPath.substr(0, szFullPath.find_last_of('/'));
	_ASEERTION_NULCHK(tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, szFullPath.c_str(), szMtlBasePath.c_str()), "MeshCreateFailed");
	_ASEERTION_NULCHK(error.empty(), "MeshCreateFailed");
	_ASEERTION_NULCHK(shapes.size() >= 1, "MeshCreateFailed");
	for (size_t sidx = 0; sidx < shapes.size(); sidx++)
	{
		size_t offset = 0;
		for (size_t fidx = 0; fidx < shapes[sidx].mesh.num_face_vertices.size(); fidx++)
		{
			unsigned char face_vertices = shapes[sidx].mesh.num_face_vertices[fidx];
			for (unsigned char v = 0; v < face_vertices; v++)
			{
				//특정 쉐이프(shape, 모델, sidx) 의 메쉬에서 인덱스값을 기반으로 정점정보의 절대값(attrib)을 가져온다
				tinyobj::index_t index = shapes[sidx].mesh.indices[offset + v];
				float vx, vy, vz, tx, ty, nx, ny, nz;
				vx = vy = vz = tx = ty = nx = ny = nz = 0.0f;
				vx = attributes.vertices[index.vertex_index * 3 + 0];
				vy = attributes.vertices[index.vertex_index * 3 + 1];
				vz = attributes.vertices[index.vertex_index * 3 + 2];
				if (index.texcoord_index >= 0)
				{
					tx = attributes.texcoords[index.texcoord_index * 2 + 0];
					ty = attributes.texcoords[index.texcoord_index * 2 + 1];
					//// V 텍스처 좌표를 왼손 시스템으로 반전시킵니다.
					//texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
					tx = 1.0f - tx;
					ty = 1.0f - ty;
				}
				
				if (index.normal_index >= 0)
				{
					nx = attributes.normals[index.normal_index * 3 + 0];
					ny = attributes.normals[index.normal_index * 3 + 1];
					nz = attributes.normals[index.normal_index * 3 + 2];
				}
				m_Points.push_back({ vx, vy, vz });
				m_Vertices.push_back({ {vx, vy, vz}, {tx, ty}, {nx, ny, nz} });
				m_Indices.push_back((UINT)m_Vertices.size()-1);
			}
			offset += face_vertices;
		}
	}
}

Mesh::~Mesh()
{
}

const std::vector<Vector3>* Mesh::GetPoints()
{
	return &m_Points;
}

Vertex_PTN* Mesh::GetVertices()
{
	return m_Vertices.data();
}

size_t Mesh::GetVerticesSize() const
{
	return m_Vertices.size();
}

UINT* Mesh::GetIndices()
{
	return m_Indices.data();
}

size_t Mesh::GetIndicesSize() const
{
	return m_Indices.size();
}

void Mesh::SetVB(size_t hashVB)
{
	m_lVB = hashVB;
}

size_t Mesh::GetVB() const
{
	return m_lVB;
}

void Mesh::SetIB(size_t hashIB)
{
	m_lIB = hashIB;
}

size_t Mesh::GetIB() const
{
	return m_lIB;
}

void Mesh::SetCL(size_t hashCL)
{
	m_lCL = hashCL;
}

size_t Mesh::GetCL() const
{
	return m_lCL;
}
