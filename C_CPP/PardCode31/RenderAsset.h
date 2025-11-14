#pragma once
#include "CommonHeader.h"

class RenderAsset
{
public:
	RenderAsset(const std::wstring& szName);
	~RenderAsset();
	RenderAsset(const RenderAsset&) = delete;
	RenderAsset& operator=(const RenderAsset&) = delete;
	RenderAsset(RenderAsset&&) = delete;
	RenderAsset& operator=(RenderAsset&&) = delete;
	void SetMeshMaterial(Mesh_Material mesh_mat);
	const std::vector<Mesh_Material>& GetMeshMats() const;
private:
	std::vector<Mesh_Material> m_MeshMaterials;
	std::wstring m_szName;
};
