#pragma once
#include "BaseSystem.h"
#include "Archetype.h"

class Entity;
class ECSSystem : public BaseSystem<ECSSystem>
{
	friend class BaseSystem<ECSSystem>;	//CRTP������ ���� friend����
private:
	ECSSystem();
	ECSSystem(const ECSSystem&) = delete;
	ECSSystem& operator=(const ECSSystem&) = delete;
	ECSSystem(ECSSystem&&) = delete;
	ECSSystem& operator=(ECSSystem&&) = delete;

	template<typename... Comps>
	ArchetypeKey CreateArchetype()
	{
		ArchetypeKey key;
		(key.set(ComponentType::GetMask<Comps>()), ...);
		(m_Archetypes[key].RegisterComponent<Comps>(), ...);
		return key;
	}
public:
	~ECSSystem();
	void Init();

private:
	std::unordered_map<ArchetypeKey, Archetype> m_Archetypes;
	std::vector<Entity*> m_Entitys;
};
#define _ECSSystem ECSSystem::GetInstance()

