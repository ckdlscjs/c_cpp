#pragma once
#include "Resource.h"
class BaseMesh : public BaseResource<BaseMesh>
{
	friend class BaseResource<BaseMesh>;
protected:
	BaseMesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>&& points, std::vector<RenderCounts>&& countsVertices, std::vector<UINT>&& indices, std::vector<RenderCounts>&& countsIndices);
	BaseMesh(const BaseMesh&) = delete;
	BaseMesh& operator=(const BaseMesh&) = delete;
	BaseMesh(BaseMesh&&) = delete;
	BaseMesh& operator=(BaseMesh&&) = delete;
public:
	virtual ~BaseMesh();
	const std::vector<std::vector<Vector3>>& GetPoints() const;
	UINT* GetIndices();
	size_t GetIndicesSize();
	void SetCL(size_t hashCL);
	const std::unordered_set<size_t>& GetCL() const;
	const std::vector<RenderCounts>& GetRendVertices() const;
	const std::vector<RenderCounts>& GetRendIndices() const;
	void SetVB(size_t hashVB);
	size_t GetVB() const;
	void SetIB(size_t hashIB);
	size_t GetIB() const;

protected:
	std::vector<std::vector<Vector3>> m_Points;
	std::vector<UINT> m_Indices;
	std::unordered_set<size_t> m_lCL;
	std::vector<RenderCounts> m_RenderVertices;
	std::vector<RenderCounts> m_RenderIndices;
	size_t m_lVB;
	size_t m_lIB;
};

inline BaseMesh::BaseMesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>&& points, std::vector<RenderCounts>&& countsVertices, std::vector<UINT>&& indices, std::vector<RenderCounts>&& countsIndices) :
	m_Points(std::move(points)),
	m_RenderVertices(std::move(countsVertices)),
	m_Indices(std::move(indices)),
	m_RenderIndices(std::move(countsIndices))
{
}

inline BaseMesh::~BaseMesh()
{
}

inline const std::vector<std::vector<Vector3>>& BaseMesh::GetPoints() const
{
	return m_Points;
}

inline UINT* BaseMesh::GetIndices()
{
	return m_Indices.data();
}

inline size_t BaseMesh::GetIndicesSize()
{
	return m_Indices.size();
}

inline void BaseMesh::SetVB(size_t hashVB)
{
	m_lVB = hashVB;
}

inline size_t BaseMesh::GetVB() const
{
	return m_lVB;
}

inline void BaseMesh::SetIB(size_t hashIB)
{
	m_lIB = hashIB;
}

inline size_t BaseMesh::GetIB() const
{
	return m_lIB;
}

inline void BaseMesh::SetCL(size_t hashCL)
{
	m_lCL.insert(hashCL);
}

inline const std::unordered_set<size_t>& BaseMesh::GetCL() const
{
	return m_lCL;
}

inline const std::vector<RenderCounts>& BaseMesh::GetRendVertices() const
{
	return m_RenderVertices;
}

inline const std::vector<RenderCounts>& BaseMesh::GetRendIndices() const
{
	return m_RenderIndices;
}

template<typename T>
class Mesh : public BaseMesh
{
public:
	Mesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>&& points, std::vector<T>&& vertices, std::vector<RenderCounts>&& countsVertices, std::vector<UINT>&& indices, std::vector<RenderCounts>&& countIndices);
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;
public:
	T* GetVertices();
	size_t GetVerticesSize();
private:
	std::vector<T> m_Vertices;
};

//정점 재계산후 우측값참조를 통해 Mesh쪽으로 넘겨받아 전달로 빠르게 값을 이동시킨다
template<typename T>
inline Mesh<T>::Mesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>&& points, std::vector<T>&& vertices, std::vector<RenderCounts>&& countsVertices, std::vector<UINT>&& indices, std::vector<RenderCounts>&& countsIndices) :
	BaseMesh(hash, szFilePath, std::move(points), std::move(countsVertices), std::move(indices), std::move(countsIndices)),
	m_Vertices(std::move(vertices))
{
	this->SetHash(hash);
	this->SetFilePath(szFilePath);
}

template<typename T>
inline T* Mesh<T>::GetVertices()
{
	return m_Vertices.data();
}
template<typename T>
inline size_t Mesh<T>::GetVerticesSize()
{
	return m_Vertices.size();
}