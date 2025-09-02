#pragma once
#include "Resource.h"

class Mesh : public BaseResource<Mesh>
{
	friend class BaseResource<Mesh>;
public:
	Mesh(size_t hashID, const std::wstring& szFilePath);
	~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;
	
	const std::vector<Vector3>* GetPoss();
	Vertex_PTN* GetVertices();
	size_t GetVerticesSize() const;
	UINT* GetIndices();
	size_t GetIndicesSize() const;
	void SetIdx_VB(size_t idxVB);
	size_t GetIdx_VB() const;
	void SetIdx_IB(size_t idxIB);
	size_t GetIdx_IB() const;
	void SetIdx_IL(size_t idxIL);
	size_t GetIdx_IL() const;
	void SetIdx_Collider(size_t idxCollider);
	size_t GetIdx_Collider() const;
private:
	std::vector<Vector3> m_Poss;
	std::vector<Vertex_PTN> m_Vertices;
	std::vector<UINT> m_Indices;
	size_t m_IdxVB;
	size_t m_IdxIB;
	size_t m_IdxIL;
	size_t m_IdxCollider;
};

