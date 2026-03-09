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
	const std::vector<std::vector<Vector3>>& GetPointsByMeshs() const;
	const std::vector<std::vector<Vector3>>& GetPointsByBones() const;
	const std::vector<UINT>& GetIndicies();
	void SetCL(size_t hashCL);
	const std::vector<size_t>& GetCLs() const;
	const std::vector<RenderCounts>& GetRendVertices() const;
	const std::vector<RenderCounts>& GetRendIndices() const;
	virtual const Vector3& GetPosition(UINT idx) const = 0;
	virtual const std::pair<std::array<UINT, 4>, std::array<float, 4>> GetBW(UINT idx) const = 0;
	void SetVB(size_t hashVB);
	size_t GetVB() const;
	void SetIB(size_t hashIB);
	size_t GetIB() const;

protected:
	std::vector<std::vector<Vector3>>							m_Points_Bones;
	std::vector<std::vector<Vector3>>							m_Points_Meshs;
	std::vector<UINT>											m_Indicies;
	std::vector<RenderCounts>									m_RenderVertices;
	std::vector<RenderCounts>									m_RenderIndices;
	size_t														m_lVB;
	size_t														m_lIB;
	std::vector<size_t>											m_lCLs;
};
inline BaseMesh::BaseMesh(size_t hash, const std::wstring& szFilePath) : BaseResource(hash, szFilePath), m_lVB(0), m_lIB(0)
{
}

inline BaseMesh::~BaseMesh()
{
}

inline const std::vector<std::vector<Vector3>>& BaseMesh::GetPointsByMeshs() const
{
	return m_Points_Meshs;
}

inline const std::vector<std::vector<Vector3>>& BaseMesh::GetPointsByBones() const
{
	return m_Points_Bones;
}

inline const std::vector<UINT>& BaseMesh::GetIndicies()
{
	return m_Indicies;
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
	m_lCLs.push_back(hashCL);
}

inline const std::vector<size_t>& BaseMesh::GetCLs() const
{
	return m_lCLs;
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
	Mesh(size_t hash, const std::wstring& szFilePath, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs, const std::vector<std::vector<Vector3>>& pointsByBones);
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;
public:
	T* GetVertices();
	size_t GetVerticesSize();
	const Vector3& GetPosition(UINT idx) const override;
	const std::pair<std::array<UINT, 4>, std::array<float, 4>> GetBW(UINT idx) const override;
private:
	std::vector<T> m_Vertices;
};

template<typename T>
inline Mesh<T>::Mesh(size_t hash, const std::wstring& szFilePath, const std::map<UINT, std::vector<Vertex_PTNTB_Skinned>>& verticesByMaterial, const std::map<UINT, std::vector<UINT>>& indicesByMaterial, const std::vector<std::vector<Vector3>>& pointsByMeshs, const std::vector<std::vector<Vector3>>& pointsByBones)
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
			m_Indicies.push_back(psumOffset + vidx);
		psumOffset += (UINT)verticesByMaterial.find(iter.first)->second.size();
	}

	m_Points_Meshs = pointsByMeshs;
	m_Points_Bones = pointsByBones;
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
inline const Vector3& Mesh<T>::GetPosition(UINT idx) const
{
	_ASEERTION_NULCHK(idx < m_Indicies.size(), "Out of Bound");
	return m_Vertices[m_Indicies[idx]].pos0;
}

template<typename T>
inline const std::pair<std::array<UINT, 4>, std::array<float, 4>> Mesh<T>::GetBW(UINT idx) const
{
	_ASEERTION_NULCHK(idx < m_Indicies.size(), "Out of Bound");
	if constexpr (std::is_same_v<T, Vertex_PT> || std::is_same_v<T, Vertex_PTN> || std::is_same_v<T, Vertex_PTNTB>)
	{
		return { {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f} };
	}
	else if constexpr (std::is_same_v<T, Vertex_PTN_Skinned> || std::is_same_v<T, Vertex_PTNTB_Skinned>)
	{
		return { m_Vertices[m_Indicies[idx]].bones, m_Vertices[m_Indicies[idx]].weights };
	}
}

