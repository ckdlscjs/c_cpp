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

	size_t HashFilePath(const std::wstring& path);
	template<typename T>
	size_t CreateResourceFromFile(const std::wstring& szFilePath);
	template<typename T>
	T* GetResource(size_t id);
	
private:
	std::unordered_map<size_t, Resource*> m_Resources;
};
#define _ResourceSystem ResourceSystem::GetInstance()

template<typename T>
size_t ResourceSystem::CreateResourceFromFile(const std::wstring& szFilePath)
{
	size_t hashID = HashFilePath(szFilePath);						
	if (m_Resources.find(hashID) != m_Resources.end()) return hashID;	
	//��ü������ �����̳ʿ� ���
	T* newResource = new T(hashID, szFilePath);
	_ASEERTION_NULCHK(newResource, typeid(T).name());
	m_Resources[hashID] = newResource;
	return hashID;
}

template<typename T>
T* ResourceSystem::GetResource(size_t id)
{
	if (m_Resources.find(id) == m_Resources.end()) return nullptr;
	return static_cast<T*>(m_Resources[id]);
}

