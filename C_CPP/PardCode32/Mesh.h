#pragma once
#include "Resource.h"
class BaseMesh : public BaseResource<BaseMesh>
{
	friend class BaseResource<BaseMesh>;
protected:
	BaseMesh(size_t hash, const std::wstring& szFilePath);
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
	std::vector<RenderCounts> m_RenderVertices;
	std::vector<RenderCounts> m_RenderIndices;
	size_t m_lVB;
	size_t m_lIB;
	std::unordered_set<size_t> m_lCL;
};
inline BaseMesh::BaseMesh(size_t hash, const std::wstring& szFilePath) : BaseResource(hash, szFilePath), m_lVB(0), m_lIB(0)
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
	Mesh(size_t hash, const std::wstring& szFilePath, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs);
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

template<typename T>
inline Mesh<T>::Mesh(size_t hash, const std::wstring& szFilePath, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs)
	: BaseMesh(hash, szFilePath)
{
	//Mesh로 정렬해 넣기위해 순서대로 Material에 따른 인덱싱을 재계산
	UINT vertexIdx = 0;
	for (const auto& iter : verticesByMaterial)
	{
		m_RenderVertices.push_back({ (UINT)iter.second.size(), vertexIdx });
		vertexIdx = (UINT)iter.second.size();
		if constexpr (std::is_same_v<T, Vertex_PT>)
		{
			for (const auto& v : iter.second)
			{
				Vertex_PT vPT;
				vPT.pos0 = v.pos0;
				vPT.tex0 = v.tex0;
				m_Vertices.push_back(vPT);
			}
		}
		else if constexpr (std::is_same_v<T, Vertex_PTN>)
		{
			for (const auto& v : iter.second)
			{
				Vertex_PTN vPTN;
				vPTN.pos0 = v.pos0;
				vPTN.tex0 = v.tex0;
				vPTN.normal0 = v.normal0;
				m_Vertices.push_back(vPTN);
			}
		}
		else if constexpr (std::is_same_v<T, Vertex_PTNTB>)
		{
			for (const auto& v : iter.second)
			{
				Vertex_PTNTB vPTNTB;
				vPTNTB.pos0 = v.pos0;
				vPTNTB.tex0 = v.tex0;
				vPTNTB.normal0 = v.normal0;
				vPTNTB.tangent0 = v.tangent0;
				vPTNTB.binormal0 = v.binormal0;
				m_Vertices.push_back(vPTNTB);
			}
		}
		else if constexpr (std::is_same_v<T, Vertex_PTN_Skinned>)
		{
			for (const auto& v : iter.second)
			{
				Vertex_PTN_Skinned vPTN_Skin;
				vPTN_Skin.pos0 = v.pos0;
				vPTN_Skin.tex0 = v.tex0;
				vPTN_Skin.normal0 = v.normal0;
				vPTN_Skin.bones = v.bones;
				vPTN_Skin.weights = v.weights;
				m_Vertices.push_back(vPTN_Skin);
			}
		}
		else
		{
			m_Vertices.insert(m_Vertices.end(), iter.second.begin(), iter.second.end());
		}
	}

	UINT indexIdx = 0;
	UINT psumOffset = 0;
	for (const auto& iter : indicesByMaterial)
	{
		m_RenderIndices.push_back({ (UINT)iter.second.size(), indexIdx });
		indexIdx += (UINT)iter.second.size();
		for (auto& vidx : iter.second)
			m_Indices.push_back(psumOffset + vidx);
		psumOffset += (UINT)verticesByMaterial.find(iter.first)->second.size();
	}

	m_Points = pointsByMeshs;
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