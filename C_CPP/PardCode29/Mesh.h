#pragma once
#include "Resource.h"
class BaseMesh : public BaseResource<BaseMesh>
{
	friend class BaseResource<BaseMesh>;
public:
	BaseMesh(size_t hash, const std::wstring& szFilePath);
	virtual ~BaseMesh();
	BaseMesh(const BaseMesh&) = delete;
	BaseMesh& operator=(const BaseMesh&) = delete;
	BaseMesh(BaseMesh&&) = delete;
	BaseMesh& operator=(BaseMesh&&) = delete;

	virtual UINT* GetIndices() = 0;
	virtual size_t GetIndicesSize() = 0;
	virtual void SetCL(size_t hashCL) = 0;
	virtual const std::unordered_set<size_t>& GetCL() const = 0;
	virtual void SetVB(size_t hashVB) = 0;
	virtual void SetIB(size_t hashIB) = 0;
	virtual size_t GetIB() const = 0;
	virtual size_t GetVB() const = 0;
	virtual const std::vector<std::vector<Vector3>>& GetPoints() const = 0;
	virtual const std::vector<RenderCounts>& GetRendIndices() const = 0;
	virtual const std::vector<RenderCounts>& GetRendVertices() const = 0;
};
inline BaseMesh::BaseMesh(size_t hash, const std::wstring& szFilePath)
{
	// BaseResource의 멤버 초기화
	this->SetHash(hash);
	this->SetFilePath(szFilePath);
}
inline BaseMesh::~BaseMesh()
{

}
template<typename T>
class Mesh : public BaseMesh
{
public:
	Mesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>&& points, std::vector<T>&& vertices, std::vector<RenderCounts>&& c_vertices, std::vector<UINT>&& indices, std::vector<RenderCounts>&& c_indices);
	~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;
public:
	const std::vector<std::vector<Vector3>>& GetPoints() const override;
	UINT* GetIndices() override;
	size_t GetIndicesSize() override;
	void SetCL(size_t hashCL) override;
	const std::unordered_set<size_t>& GetCL() const override;
	const std::vector<RenderCounts>& GetRendVertices() const override;
	const std::vector<RenderCounts>& GetRendIndices() const override;
	void SetVB(size_t hashVB) override;
	size_t GetVB() const override;
	void SetIB(size_t hashIB) override;
	size_t GetIB() const override;

	T* GetVertices();
	size_t GetVerticesSize();

private:
	std::vector<std::vector<Vector3>> m_Points;
	std::vector<T> m_Vertices;
	std::vector<UINT> m_Indices;
	std::unordered_set<size_t> m_lCL;
	std::vector<RenderCounts> m_RenderVertices;
	std::vector<RenderCounts> m_RenderIndices;
	size_t m_lVB;
	size_t m_lIB;
};

//정점 재계산후 우측값참조를 통해 Mesh쪽으로 넘겨받아 전달로 빠르게 값을 이동시킨다
template<typename T>
inline Mesh<T>::Mesh(size_t hash, const std::wstring& szFilePath, std::vector<std::vector<Vector3>>&& points, std::vector<T>&& vertices, std::vector<RenderCounts>&& countsVertices, std::vector<UINT>&& indices, std::vector<RenderCounts>&& countsIndices)
	: BaseMesh(hash, szFilePath),
	m_Points(std::move(points)),
	m_Vertices(std::move(vertices)),
	m_RenderVertices(std::move(countsVertices)),
	m_Indices(std::move(indices)),
	m_RenderIndices(std::move(countsIndices))
{

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