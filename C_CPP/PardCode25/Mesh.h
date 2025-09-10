#pragma once
#include "Resource.h"

class Mesh : public BaseResource<Mesh>
{
	friend class BaseResource<Mesh>;
public:
	Mesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>* points, std::vector<std::vector<Vertex_PTN>>* vertices, std::vector<std::vector<UINT>>* indices);
	~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;
	
	const std::vector<std::vector<Vector3>>& GetPoints() const;
	Vertex_PTN* GetVertices();
	size_t GetVerticesSize();
	UINT* GetIndices();
	size_t GetIndicesSize();
	void SetCL(size_t hashCL);
	const std::vector<size_t>& GetCL() const;

	void SetVB(size_t hashVB);
	size_t GetVB() const;
	void SetIB(size_t hashIB);
	size_t GetIB() const;
	
private:
	std::vector<std::vector<Vector3>> m_Points;
	std::vector<std::pair<UINT, UINT>> m_VIsizes;
	std::vector<Vertex_PTN> m_Vertices;
	std::vector<UINT> m_Indices;
	std::vector<size_t> m_lCL;
	size_t m_lVB;
	size_t m_lIB;
};

