#pragma once
#include "Resource.h"

template<typename T>
class Mesh : public BaseResource<Mesh<T>>
{
	friend class BaseResource<Mesh<T>>;
public:
	Mesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>&& points, std::vector<T>&& vertices, std::vector<RenderCounts>&& c_vertices, std::vector<UINT>&& indices, std::vector<RenderCounts>&& c_indices);
	~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;
	
	const std::vector<std::vector<Vector3>>& GetPoints() const;
	T* GetVertices();
	size_t GetVerticesSize();
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
	
private:
	std::vector<std::vector<Vector3>> m_Points;
	std::vector<RenderCounts> m_RenderVertices;
	std::vector<RenderCounts> m_RenderIndices;
	std::vector<T> m_Vertices;
	std::vector<UINT> m_Indices;
	std::unordered_set<size_t> m_lCL;
	size_t m_lVB;
	size_t m_lIB;
};

//정점 재계산후 우측값참조를 통해 Mesh쪽으로 넘겨받아 전달로 빠르게 값을 이동시킨다
template<typename T>
inline Mesh<T>::Mesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>&& points, std::vector<T>&& vertices, std::vector<RenderCounts>&& countsVertices, std::vector<UINT>&& indices, std::vector<RenderCounts>&& countsIndices)
	: m_Points(std::move(points)),
	m_Vertices(std::move(vertices)),
	m_RenderVertices(std::move(countsVertices)),
	m_Indices(std::move(indices)),
	m_RenderIndices(std::move(countsIndices))
{
	// BaseResource의 멤버 초기화
	this->SetHash(hash);
	this->SetFilePath(szFilePath);
}

template<typename T>
inline Mesh<T>::~Mesh()
{
}

template<typename T>
inline const std::vector<std::vector<Vector3>>& Mesh<T>::GetPoints() const
{
	return m_Points;
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
template<typename T>
inline UINT* Mesh<T>::GetIndices()
{
	return m_Indices.data();
}
template<typename T>
inline size_t Mesh<T>::GetIndicesSize()
{
	return m_Indices.size();
}
template<typename T>
inline void Mesh<T>::SetVB(size_t hashVB)
{
	m_lVB = hashVB;
}
template<typename T>
inline size_t Mesh<T>::GetVB() const
{
	return m_lVB;
}
template<typename T>
inline void Mesh<T>::SetIB(size_t hashIB)
{
	m_lIB = hashIB;
}
template<typename T>
inline size_t Mesh<T>::GetIB() const
{
	return m_lIB;
}
template<typename T>
inline void Mesh<T>::SetCL(size_t hashCL)
{
	m_lCL.insert(hashCL);
}
template<typename T>
inline const std::unordered_set<size_t>& Mesh<T>::GetCL() const
{
	return m_lCL;
}
template<typename T>
inline const std::vector<RenderCounts>& Mesh<T>::GetRendVertices() const
{
	return m_RenderVertices;
}
template<typename T>
inline const std::vector<RenderCounts>& Mesh<T>::GetRendIndices() const
{
	return m_RenderIndices;
}
