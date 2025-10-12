#pragma once
#include "BaseSystem.h"
#include "Mesh.h"

class Material;
class Resource;
class ResourceSystem : public BaseSystem<ResourceSystem>
{
	friend class BaseSystem<ResourceSystem>;	//CRTP패턴을 위한 friend선언
private:
	ResourceSystem();
	ResourceSystem(const ResourceSystem&) = delete;
	ResourceSystem& operator=(const ResourceSystem&) = delete;
	ResourceSystem(ResourceSystem&&) = delete;
	ResourceSystem& operator=(ResourceSystem&&) = delete;
	
public:
	~ResourceSystem();
	void Init();
	template<typename T, typename... Types>
	T* CreateResourceFromFile(const std::wstring& szFilePath, Types&&... args);
	template<typename T, typename... Types>
	std::vector<T*> CreateResourcesFromFile(const std::wstring& szFilePath, Types&&... args);
	template<typename T>
	T* GetResource(size_t hash);
	
private:
	std::unordered_map<size_t, Resource*> m_Resources;
};
#define _ResourceSystem ResourceSystem::GetInstance()

template<typename T, typename... Types>
T* ResourceSystem::CreateResourceFromFile(const std::wstring& szFilePath, Types&&... args)
{
	size_t hash = Hasing_wstring(szFilePath);
	if (m_Resources.find(hash) != m_Resources.end()) return static_cast<T*>(m_Resources[hash]);
	//객체생성후 컨테이너에 등록
	T* newResource = new T(hash, szFilePath, std::forward<Types>(args)...);
	_ASEERTION_NULCHK(newResource, typeid(T).name());
	m_Resources[hash] = newResource;
	return newResource;
}

template<typename T, typename ...Types>
inline std::vector<T*> ResourceSystem::CreateResourcesFromFile(const std::wstring& szFilePath, Types && ...args)
{
	return std::vector<T*>();
}

template<typename T>
T* ResourceSystem::GetResource(size_t hash)
{
	_ASEERTION_NULCHK(m_Resources.find(hash) != m_Resources.end(), "hash is null");
	return static_cast<T*>(m_Resources[hash]);
}

//템플릿특수화
template<>
Mesh<Vertex_PTN>* ResourceSystem::CreateResourceFromFile<Mesh<Vertex_PTN>>(const std::wstring& szFilePath);

template<>
Mesh<Vertex_PTNTB>* ResourceSystem::CreateResourceFromFile<Mesh<Vertex_PTNTB>>(const std::wstring& szFilePath);

template<>
std::vector<Material*> ResourceSystem::CreateResourcesFromFile<Material>(const std::wstring& szFilePath);
