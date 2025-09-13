#include "Mesh.h"

Mesh::Mesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>& points, std::vector<std::vector<Vertex_PTN>>& vertices, std::vector<std::vector<UINT>>& indices)
{
	SetHash(hash);
	SetFilePath(szFilePath);
	m_Points = points;
	UINT idx = 0;
	for (int i = 0; i < vertices.size(); i++)
	{
		m_RenderVertices.push_back({ (UINT)vertices[i].size(), idx });
		idx += (UINT)vertices[i].size();
	}
	idx = 0;
	for (int i = 0; i < indices.size(); i++)
	{
		m_RenderIndices.push_back({ (UINT)indices[i].size(), idx });
		idx += (UINT)indices[i].size();
	}
	for (const auto& iter : vertices)
		m_Vertices.insert(m_Vertices.end(), iter.begin(), iter.end());
	UINT psum = 0;
	for (int i = 0; i < indices.size(); i++)
	{
		for (int j = 0; j < indices[i].size(); j++)
			m_Indices.push_back(psum + indices[i][j]);
		psum += vertices[i].size();
	}
}

Mesh::Mesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>& points, std::vector<std::vector<Vertex_PTN>>& vertices, std::vector<UINT>& indices)
{
	SetHash(hash);
	SetFilePath(szFilePath);
	m_Points = points;
	UINT idx = 0;
	for (int i = 0; i < vertices.size(); i++)
	{
		m_RenderVertices.push_back({ (UINT)vertices[i].size(), idx  });
		m_RenderIndices.push_back({ (UINT)vertices[i].size(), idx });
		idx += (UINT)vertices[i].size();
	}
	for (const auto& iter : vertices)
		m_Vertices.insert(m_Vertices.end(), iter.begin(), iter.end());
	m_Indices = indices;
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
	m_lCL.insert(hashCL);
}

const std::unordered_set<size_t>& Mesh::GetCL() const
{
	return m_lCL;
}

const std::vector<CountRender>& Mesh::GetRendVertices() const
{
	return m_RenderVertices;
}

const std::vector<CountRender>& Mesh::GetRendIndices() const
{
	return m_RenderIndices;
}
