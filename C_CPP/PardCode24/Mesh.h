#pragma once
#include "Resource.h"

class Mesh : public BaseResource<Mesh>
{
	friend class BaseResource<Mesh>;
public:
	Mesh(size_t hash, const std::wstring& szFilePath);
	~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;
	
	const std::vector<Vector3>* GetPoints();
	Vertex_PTN* GetVertices();
	size_t GetVerticesSize() const;
	UINT* GetIndices();
	size_t GetIndicesSize() const;

	void SetVB(size_t hashVB);
	size_t GetVB() const;
	void SetIB(size_t hashIB);
	size_t GetIB() const;
	void SetIL(size_t hashIL);
	size_t GetIL() const;
	void SetCL(size_t hashCL);
	size_t GetCL() const;
private:
	std::vector<Vector3> m_Points;
	std::vector<Vertex_PTN> m_Vertices;
	std::vector<UINT> m_Indices;
	size_t m_lVB;
	size_t m_lIB;
	size_t m_lIL;
	size_t m_lCL;
};

