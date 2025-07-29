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
	
	void* GetVertices() const;
	size_t GetVerticesSize() const;
	void* GetIndices() const;
	size_t GetIndicesSize() const;
	void SetIdx_VB(size_t idxVB);
	size_t GetIdx_VB() const;
	void SetIdx_IB(size_t idxIB);
	size_t GetIdx_IB() const;
	void SetIdx_VS(size_t idxVS);
	size_t GetIdx_VS() const;
	void SetIdx_PS(size_t idxPS);
	size_t GetIdx_PS() const;
	void SetIdx_IL(size_t idxIL);
	size_t GetIdx_IL() const;
private:
	std::vector<Vertex_PTN> m_Vertices;
	std::vector<UINT> m_Indices;
	size_t m_IdxVB;
	size_t m_IdxIB;
	size_t m_IdxVS;
	size_t m_IdxPS;
	size_t m_IdxIL;
};

