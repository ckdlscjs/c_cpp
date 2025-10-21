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
	std::vector<T>& GetComponents(ArchetypeKey key);

	void DeleteEntity(size_t entityIdx);
private:
	std::unordered_map<ArchetypeKey, Archetype> m_Archetypes;
	std::vector<Entity> m_Entitys;
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
	m_Entitys.push_back(Entity());
	m_Entitys.back().m_szName = szName;
	m_Entitys.back().m_Key = CreateArchetype<Comps...>();
	m_Entitys.back().m_ChunkIdx = m_Entitys.size() - 1;
	return m_Entitys.size()-1;
}

template<typename T>
inline void ECSSystem::AddComponent(ArchetypeKey key, T&& component)
{
	m_Archetypes[key].AddComponent<T>(std::forward<T>(component));
}

template<typename T>
inline std::vector<T>& ECSSystem::GetComponents(ArchetypeKey key)
{
	return m_Archetypes[key].GetComponents<T>();
}

