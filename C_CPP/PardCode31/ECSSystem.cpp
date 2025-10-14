#include "ECSSystem.h"
#include "Components.h"

ECSSystem::ECSSystem()
{
}

ECSSystem::~ECSSystem()
{
	for (auto iter = m_Entitys.begin(); iter != m_Entitys.end();)
	{
		delete *iter;
		iter = m_Entitys.erase(iter);
	}
}

void ECSSystem::Init()
{
	//auto key1 = CreateArchetype<Component_Transform, Component_Input>();
	/*Component_Transform moves;
	moves.m_vPosition.SetX(1.0f);
	m_Archetypes[key1].AddComponent<Component_Transform>(std::move(moves));
	std::cout << m_Archetypes[key1].GetComponents<Component_Transform>()[0].m_vPosition.GetX() << '\n';
	for (int i = 0; i <= 100; i++)
	{
		float chk = static_cast<float>(i);
		Component_Transform transform;
		transform.m_vPosition = Vector3(i, i, i);
		transform.m_vRotate = Vector3(i, i, i);
		transform.m_vScale = Vector3(i, i, i);
		m_Archetypes[key1].AddComponent<Component_Transform>(std::move(transform));
		std::cout << m_Archetypes[key1].GetComponents<Component_Transform>()[i].m_vPosition.GetX() << '\n';
	}
	*/
}

void ECSSystem::DeleteEntity(size_t idx)
{
	if (idx >= m_lEntitysCount) return;
	m_Archetypes[m_Entitys[idx]->m_Key].DeleteComponent(idx);
	m_Entitys[idx]->m_szName = m_Entitys.back()->m_szName;
	m_Entitys[idx]->m_Key = m_Entitys.back()->m_Key;
	m_Entitys[idx]->m_lIdx = idx;
	m_lEntitysCount--;
}