#include "Mesh.h"
Mesh::Mesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>* points, std::vector<std::vector<Vertex_PTN>>* vertices, std::vector<std::vector<UINT>>* indices)
{
	SetHash(hash);
	SetFilePath(szFilePath);
	m_Points = *points;
	for (int i = 0; i < (*vertices).size(); i++)
		m_VIsizes.push_back({ (UINT)vertices[i].size(), (UINT)indices[i].size() });
	for (const auto& iter : *vertices)
		m_Vertices.insert(m_Vertices.end(), iter.begin(), iter.end());
	for (const auto& iter : *indices)
		m_Indices.insert(m_Indices.end(), iter.begin(), iter.end());
}

Mesh::~Mesh()
{
}

const std::vector<std::vector<Vector3>>& Mesh::GetPoints() const
{
	return m_Points;
}

Vertex_PTN* Mesh::GetVertices()
{
	return m_Vertices.data();
}

size_t Mesh::GetVerticesSize()
{
	return m_Vertices.size();
}

UINT* Mesh::GetIndices()
{
	return m_Indices.data();
}

size_t Mesh::GetIndicesSize()
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
	m_lCL.push_back(hashCL);
}

const std::vector<size_t>& Mesh::GetCL() const
{
	return m_lCL;
}
