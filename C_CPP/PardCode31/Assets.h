#pragma once
#include "CommonHeader.h"
template<typename T>
class BaseAsset
{
public:
	BaseAsset(const std::wstring& szName);
	~BaseAsset();
	BaseAsset(const BaseAsset&) = delete;
	BaseAsset& operator=(const BaseAsset&) = delete;
	BaseAsset(BaseAsset&&) = delete;
	BaseAsset& operator=(BaseAsset&&) = delete;
	const std::wstring& GetName() const;
private:
	std::wstring m_szName;
};

template<typename T>
inline BaseAsset<T>::BaseAsset(const std::wstring& szName) : m_szName(szName)
{
	std::cout << "Initialize : " << "BaseAsset <" << typeid(T).name() << "> Class" << '\n';
}

template<typename T>
inline BaseAsset<T>::~BaseAsset()
{
	std::cout << "Release : " << "BaseAsset <" << typeid(T).name() << "> Class" << '\n';
}

template<typename T>
inline const std::wstring& BaseAsset<T>::GetName() const
{
	return m_szName;
}

class RenderAsset : public BaseAsset<RenderAsset>
{
	friend class BaseAsset<RenderAsset>;
public:
	RenderAsset(const std::wstring& szName);
	RenderAsset(const RenderAsset&) = delete;
	RenderAsset& operator=(const RenderAsset&) = delete;
	RenderAsset(RenderAsset&&) = delete;
	RenderAsset& operator=(RenderAsset&&) = delete;
	std::vector<Mesh_Material> m_hMeshMats;
};

class ColliderAsset : public BaseAsset<ColliderAsset>
{
	friend class BaseAsset<ColliderAsset>;
public:
	ColliderAsset(const std::wstring& szName);
	ColliderAsset(const ColliderAsset&) = delete;
	ColliderAsset& operator=(const ColliderAsset&) = delete;
	ColliderAsset(ColliderAsset&&) = delete;
	ColliderAsset& operator=(ColliderAsset&&) = delete;
	std::unordered_set<size_t> m_hColliders;
};
