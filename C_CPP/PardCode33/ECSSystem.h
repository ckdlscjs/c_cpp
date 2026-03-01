#pragma once
#include "BaseSystem.h"
#include "Archetype.h"
#include "Entity.h"
#include "Components.h"

/*
* 251108 수정필요, 일단 렌더러의 남은구현을위해 정렬까지만하고 추후 최적화 를 구현한다
* 업데이트관련된 사항을 다음과같이 최적화한다
* 1.업데이트관련(Input등) 을 인덱스기반으로 뒤로스왑해정리한다
* 2.해당청크들만을 업데이트종류에따라 정리한다
* 3.함수포인터로 지정된 함수들에 기반해 업데이트를 수행한다
*/

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
	T& GetComponent(size_t idx_lookup);
	std::vector<Archetype*> QueryArchetypes(ArchetypeKey key);
	void UpdateSwapChunk(const std::vector<std::pair<size_t, size_t>>& swapRowCols, size_t startIdx, Archetype* archetype);
	void DeleteEntity(size_t lookupIdx);
private:
	size_t FindEntity(size_t lookupIdx);
private:
	std::unordered_map<ArchetypeKey, Archetype*> m_Archetypes;
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
	m_Archetypes[key] = new Archetype();
	(m_Archetypes[key]->RegisterComponent<Comps>(), ...);
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
	if (m_Archetypes[key]->NeedNewChunk())
		(m_Archetypes[key]->CreateNewChunk<Comps>(), ...);
	m_Archetypes[key]->ReserveIndexes(m_Entitys.back().m_IdxLookup, m_Entitys.back().m_IdxRow, m_Entitys.back().m_IdxCol);
	return lookupIdx;
}

template<typename T>
inline void ECSSystem::AddComponent(ArchetypeKey key, T&& component)
{
	m_Archetypes[key]->AddComponent<T>(std::forward<T>(component));
}

template<typename T>
T& ECSSystem::GetComponent(size_t idx_lookup)
{
	size_t entityIdx = FindEntity(idx_lookup);
	const auto& entity = m_Entitys[entityIdx];
	return m_Archetypes[entity.m_Key]->GetComponents<T>(entity.m_IdxRow)[entity.m_IdxCol];
}

