#pragma once
#include "BaseSystem.h"
#include "Archetype.h"
#include "Entity.h"
class Entity;
class ECSSystem : public BaseSystem<ECSSystem>
{
	friend class BaseSystem<ECSSystem>;	//CRTP패턴을 위한 friend선언
private:
	ECSSystem();
	ECSSystem(const ECSSystem&) = delete;
	ECSSystem& operator=(const ECSSystem&) = delete;
	ECSSystem(ECSSystem&&) = delete;
	ECSSystem& operator=(ECSSystem&&) = delete;

	template<typename... Comps>
	ArchetypeKey CreateArchetype();
public:
	~ECSSystem();
	void Init();
	template<typename... Comps>
	size_t CreateEntity(const std::wstring& szName);
	template<typename T>
	void AddComponent(size_t idx, T&& component);

	template<typename T>
	T* GetComponents(size_t idx);
	void DeleteEntity(size_t idx);
private:
	std::unordered_map<ArchetypeKey, Archetype> m_Archetypes;
	std::vector<Entity*> m_Entitys;
	size_t m_lEntitysCount = 0;
};
#define _ECSSystem ECSSystem::GetInstance()

template<typename ...Comps>
inline ArchetypeKey ECSSystem::CreateArchetype()
{
	ArchetypeKey key;
	(key.set(ComponentType::GetMask<Comps>()), ...);
	if (m_Archetypes.find(key) != m_Archetypes.end()) 
		return key;
	(m_Archetypes[key].RegisterComponent<Comps>(), ...);
	return key;
}

template<typename ...Comps>
inline size_t ECSSystem::CreateEntity(const std::wstring& szName)
{
	Entity* newEntity;
	if (m_Entitys.size() <= m_lEntitysCount)
	{
		newEntity = new Entity();
		m_Entitys.push_back(newEntity);
	}
	else
	{
		newEntity = m_Entitys.back();
	}
	size_t idx = m_lEntitysCount++;
	newEntity->m_szName = szName;
	newEntity->m_Key = CreateArchetype<Comps...>();
	newEntity->m_lIdx = idx;
	return idx;
}

template<typename T>
inline void ECSSystem::AddComponent(size_t idx, T&& component)
{
	m_Archetypes[m_Entitys[idx]->m_Key].AddComponent<T>(std::forward<T>(component));
}

template<typename T>
inline T* ECSSystem::GetComponents(size_t idx)
{
	return m_Archetypes[m_Entitys[idx]->m_Key].GetComponents<T>();
}
