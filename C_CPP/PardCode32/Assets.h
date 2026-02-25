#include "Resource.h"

class RenderAsset : public BaseResource<RenderAsset>
{
	friend class BaseResource<RenderAsset>;
public:
	RenderAsset(size_t hash, const std::wstring& szName);
	RenderAsset(const RenderAsset&) = delete;
	RenderAsset& operator=(const RenderAsset&) = delete;
	RenderAsset(RenderAsset&&) = delete;
	RenderAsset& operator=(RenderAsset&&) = delete;
	std::vector<Mesh_Material> m_hMeshMats;
};
inline RenderAsset::RenderAsset(size_t hash, const std::wstring& szFilePath) : BaseResource(hash, szFilePath)
{
}

/*
class ColliderAsset : public BaseResource<ColliderAsset>
{
	friend class BaseResource<ColliderAsset>;
public:
	ColliderAsset(size_t hash, const std::wstring& szName);
	ColliderAsset(const ColliderAsset&) = delete;
	ColliderAsset& operator=(const ColliderAsset&) = delete;
	ColliderAsset(ColliderAsset&&) = delete;
	ColliderAsset& operator=(ColliderAsset&&) = delete;
	std::unordered_set<size_t> m_hColliders;
};
inline ColliderAsset::ColliderAsset(size_t hash, const std::wstring& szFilePath) : BaseResource(hash, szFilePath)
{
}
*/