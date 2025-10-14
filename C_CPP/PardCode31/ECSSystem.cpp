#include "ECSSystem.h"
#include "Components.h"

ECSSystem::ECSSystem()
{
}

ECSSystem::~ECSSystem()
{
}

void ECSSystem::Init()
{
}

void ECSSystem::DeleteEntity(size_t entityIdx)
{
	_ASEERTION_NULCHK(entityIdx < m_lEntitysCount, "idx Out of bounds");
	m_Archetypes[m_Entitys[entityIdx].m_Key].DeleteComponent(m_Entitys[entityIdx].m_ChunkIdx);
	m_Entitys[entityIdx].m_szName = m_Entitys.back().m_szName;
	m_Entitys[entityIdx].m_Key = m_Entitys.back().m_Key;
	m_lEntitysCount--;
}