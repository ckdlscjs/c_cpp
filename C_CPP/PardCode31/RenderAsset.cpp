#include "RenderAsset.h"

RenderAsset::RenderAsset(const std::wstring& szName) : m_szName(szName)
{
}

RenderAsset::~RenderAsset()
{
}

void RenderAsset::SetMeshMaterial(Mesh_Material mesh_mat)
{
	m_MeshMaterials.push_back(mesh_mat);
}
