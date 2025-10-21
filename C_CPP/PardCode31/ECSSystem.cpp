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
	m_Entitys.reserve(32);
}

void ECSSystem::DeleteEntity(size_t entityIdx)
{
	_ASEERTION_NULCHK(entityIdx < m_Entitys.size(), "idx Out of bounds");
	m_Archetypes[m_Entitys[entityIdx].m_Key].DeleteComponent(m_Entitys[entityIdx].m_ChunkIdx);
	m_Entitys[entityIdx] = std::move(m_Entitys.back());
	m_Entitys.pop_back();
}