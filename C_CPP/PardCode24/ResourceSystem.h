#pragma once
#include "BaseSystem.h"

class Resource;
class ResourceSystem : public BaseSystem<ResourceSystem>
{
	friend class BaseSystem<ResourceSystem>;	//CRTP������ ���� friend����
private:
	ResourceSystem();
	~ResourceSystem();
	ResourceSystem(const ResourceSystem&) = delete;
	ResourceSystem& operator=(const ResourceSystem&) = delete;
	ResourceSystem(ResourceSystem&&) = delete;
	ResourceSystem& operator=(ResourceSystem&&) = delete;
	
public:
	void Init();
	void Release();

	template<typename T, typename... Types>
	T* CreateResourceFromFile(const std::wstring& szFilePath, Types&&... args);
	template<typename T>
	T* GetResource(size_t hash);
	
private:
	std::unordered_map<size_t, Resource*> m_Resources;
};
#define _ResourceSystem ResourceSystem::GetInstance()

template<typename T, typename... Types>
T* ResourceSystem::CreateResourceFromFile(const std::wstring& szFilePath, Types&&... args)
{					
	size_t hash = HashingFile(szFilePath);
	if (m_Resources.find(hash) != m_Resources.end()) return static_cast<T*>(m_Resources[hash]);
	//��ü������ �����̳ʿ� ���
	T* newResource = new T(hash, szFilePath, std::forward<Types>(args)...);
	_ASEERTION_NULCHK(newResource, typeid(T).name());
	m_Resources[hash] = newResource;
	return newResource;
}

template<typename T>
T* ResourceSystem::GetResource(size_t hash)
{
	_ASEERTION_NULCHK(m_Resources.find(hash) != m_Resources.end(), "hash is null");
	return static_cast<T*>(m_Resources[hash]);
}

