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

void ECSSystem::DeleteEntity(size_t lookupIdx)
{
	size_t delEntityIdx = FindEntity(lookupIdx);
	Entity delEntity = m_Entitys[delEntityIdx];
	size_t movedLookupIdx = m_Archetypes[delEntity.m_Key].DeleteComponent(delEntity.m_IdxRow, delEntity.m_IdxCol);
	m_LookupTable[lookupIdx] = -1;	//해당 룩업은 폐기
	//삭제시킨 Lookup이 삭제시킬 Lookup과 같다면 청크의 끝이 삭제된것
	if (movedLookupIdx == lookupIdx)
	{
		//마지막 엔티티를 삭제된 엔티티의 자리로 swap and pop한다
		m_LookupTable[m_Entitys.back().m_IdxLookup] = delEntityIdx;	//마지막을 가리키는 룩업을 스왑엔티티로지정
		m_Entitys[delEntityIdx] = std::move(m_Entitys.back());		//스왑자리에 마지막엔티티를 이동
		m_Entitys.pop_back();
		return;
	}
	//옮겨진 엔티티의 row, col을 삭제된 row,col로 갱신시킨다
	size_t movEntityIdx = FindEntity(movedLookupIdx);
	m_Entitys[movEntityIdx].m_IdxRow = delEntity.m_IdxRow;
	m_Entitys[movEntityIdx].m_IdxCol = delEntity.m_IdxCol;

	//가장 끝의 엔티티를 삭제된 자리로 옮긴다(swap and pop)
	size_t lastLookupIdx = m_Entitys.back().m_IdxLookup;
	m_LookupTable[lastLookupIdx] = delEntityIdx;
	m_Entitys[delEntityIdx] = std::move(m_Entitys.back());
	m_Entitys.pop_back();
}

size_t ECSSystem::FindEntity(size_t lookupIdx)
{
	_ASEERTION_NULCHK(lookupIdx < m_LookupTable.size(), "idx Out of bounds");
	_ASEERTION_NULCHK(0 <= m_LookupTable[lookupIdx] && m_LookupTable[lookupIdx] < m_Entitys.size() , "idx Out of bounds");
	return m_LookupTable[lookupIdx];
}
