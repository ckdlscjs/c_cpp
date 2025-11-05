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
	size_t CreateEntity();
	template<typename T>
	void AddComponent(ArchetypeKey key, T&& component);
	template<typename T>
	std::vector<T>& GetComponents(ArchetypeKey key, size_t idxRow);
	std::vector<Archetype*> QueryArchetypes(ArchetypeKey key);
	void Swap(size_t srcIdx, size_t destIdx);
	void DeleteEntity(size_t lookupIdx);
private:
	size_t FindEntity(size_t lookupIdx);
private:
	std::unordered_map<ArchetypeKey, Archetype> m_Archetypes;
	std::vector<Entity> m_Entitys;
	std::vector<size_t> m_LookupTable;
};
#define _ECSSystem ECSSystem::GetInstance()

//폴드표현식을 이용해 component mask를 기반으로 ArchetypeKey를생성한다
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
inline size_t ECSSystem::CreateEntity()
{
	_ASEERTION_NULCHK(sizeof...(Comps) > 0, "Components zero");
	size_t lookupIdx = m_LookupTable.size();
	size_t entityIdx = m_Entitys.size();
	m_LookupTable.push_back(entityIdx);
	m_Entitys.push_back(Entity());
	ArchetypeKey key = CreateArchetype<Comps...>();
	m_Entitys.back().m_Key = key;
	m_Entitys.back().m_IdxLookup = lookupIdx;
	if (m_Archetypes[key].NeedNewChunk())
		(m_Archetypes[key].CreateNewChunk<Comps>(), ...);
	m_Archetypes[key].ReserveIndexes(m_Entitys.back().m_IdxLookup, m_Entitys.back().m_IdxRow, m_Entitys.back().m_IdxCol);
	return lookupIdx;
}

template<typename T>
inline void ECSSystem::AddComponent(ArchetypeKey key, T&& component)
{
	m_Archetypes[key].AddComponent<T>(std::forward<T>(component));
}

template<typename T>
inline std::vector<T>& ECSSystem::GetComponents(ArchetypeKey key, size_t idxRow)
{
	return m_Archetypes[key].GetComponents<T>(idxRow);
}

