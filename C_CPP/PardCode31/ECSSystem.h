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
	ArchetypeKey GetArchetypeKey();
	template<typename... Comps>
	size_t CreateEntity(const std::wstring& szName);
	template<typename T>
	void AddComponent(ArchetypeKey key, T&& component);
	template<typename T>
	T* GetComponents(ArchetypeKey key);

	void DeleteEntity(size_t entityIdx);
private:
	std::unordered_map<ArchetypeKey, Archetype> m_Archetypes;
	std::vector<Entity> m_Entitys;
	size_t m_lEntitysCount = 0;
};
#define _ECSSystem ECSSystem::GetInstance()

template<typename ...Comps>
inline ArchetypeKey ECSSystem::GetArchetypeKey()
{
	ArchetypeKey key;
	(key.set(ComponentType::GetMask<Comps>()), ...);
	return key;
}

template<typename ...Comps>
inline ArchetypeKey ECSSystem::CreateArchetype()
{
	ArchetypeKey key = GetArchetypeKey<Comps...>();
	if (m_Archetypes.find(key) != m_Archetypes.end()) 
		return key;
	(m_Archetypes[key].RegisterComponent<Comps>(), ...);
	return key;
}



template<typename ...Comps>
inline size_t ECSSystem::CreateEntity(const std::wstring& szName)
{
	
	if (m_Entitys.size() <= m_lEntitysCount) m_Entitys.push_back(Entity());
	size_t idx = m_lEntitysCount++;
	m_Entitys[idx].m_szName = szName;
	m_Entitys[idx].m_Key = CreateArchetype<Comps...>();
	m_Entitys[idx].m_ChunkIdx = idx;
	return idx;
}

template<typename T>
inline void ECSSystem::AddComponent(ArchetypeKey key, T&& component)
{
	m_Archetypes[key].AddComponent<T>(std::forward<T>(component));
}

template<typename T>
inline T* ECSSystem::GetComponents(ArchetypeKey key)
{
	return m_Archetypes[key].GetComponents<T>();
}
