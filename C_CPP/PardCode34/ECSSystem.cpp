#include "ECSSystem.h"
#include "Components.h"

ECSSystem::ECSSystem()
{
}

ECSSystem::~ECSSystem()
{
	for (auto iter = m_Archetypes.begin(); iter != m_Archetypes.end();)
	{
		delete iter->second;
		iter = m_Archetypes.erase(iter);
	}
}

void ECSSystem::Init()
{
}

std::vector<Archetype*> ECSSystem::QueryArchetypes(ArchetypeKey key)
{
	std::vector<Archetype*> archetypes;
	for (auto& iter : m_Archetypes)
	{
		if ((iter.first & key) == key && !iter.second->IsEmpty())
			archetypes.push_back(iter.second);
	}
	return archetypes;
}

Archetype* ECSSystem::QueryArchetype(ArchetypeKey key)
{
	_ASEERTION_NULCHK(m_Archetypes.find(key) != m_Archetypes.end(), "NotExist");
	return m_Archetypes[key];
}

/*
* RowCols에 실제로 업데이트된 엔티티들의 row/col이 순서대로 담겨있으므로 이를 역순으로 Archetype의 끝부분으로 
* 밀집시켜 이후 계층에서 순회량을 줄인다
*/
void ECSSystem::UpdateSwapChunk(const std::vector<std::pair<size_t, size_t>>& RowCols, size_t startIdx, Archetype* archetype)
{
	if (RowCols.empty()) return;
	size_t idx = archetype->GetAllChunkCount() - 1;
	size_t capacity = archetype->GetCapacity_Chunks();
	//청크의 뒤에 배치시킨다(Swap), 충돌을 방지하기위해 뒤에서부터 채워나간다
	for(auto iter = RowCols.rbegin(); iter != RowCols.rend(); iter++)
	{
		size_t srcRow = iter->first;
		size_t srcCol = iter->second;
		size_t destRow = idx / capacity;
		size_t destCol = idx % capacity;
		if (srcRow != destRow || srcCol != destCol)
		{
			auto src_dest = archetype->SwapChunkData(srcRow, srcCol, destRow, destCol);
			size_t srcEntityIdx = FindEntity(src_dest.first);
			size_t destEntityIdx = FindEntity(src_dest.second);
			// srcLookup의 데이터가 dest slot으로 이동
			m_Entitys[srcEntityIdx].m_IdxRow = destRow;
			m_Entitys[srcEntityIdx].m_IdxCol = destCol;

			// destLookup의 데이터가 src slot으로 이동
			m_Entitys[destEntityIdx].m_IdxRow = srcRow;
			m_Entitys[destEntityIdx].m_IdxCol = srcCol;
		}
		idx--;
	}
	//다음 계층에서의 사용을 위한 row/col transfer 계산
	archetype->m_transfer_row = startIdx / capacity;
	archetype->m_transfer_col = startIdx % capacity;
}

//사용된 룩업을 폐기시키고 삭제될 엔티티를 마지막 엔티티로 변경후 폐기한다
void ECSSystem::DeleteEntity(size_t lookupIdx)
{
	size_t delEntityIdx = FindEntity(lookupIdx);
	Entity delEntity = m_Entitys[delEntityIdx];
	m_LookupTable[lookupIdx] = _HashNotInitialize;	//삭제위치를 가리키는 룩업은 이제 사용하지않는다

	size_t movedLookupIdx = m_Archetypes[delEntity.m_Key]->DeleteComponent(delEntity.m_IdxRow, delEntity.m_IdxCol);

	//이동된 인덱싱과 삭제할 인덱스 룩업은 서로 같은 key이므므로 row, col을 이동시킨다
	if (movedLookupIdx != lookupIdx)
	{
		size_t movEntityIdx = FindEntity(movedLookupIdx);
		m_Entitys[movEntityIdx].m_IdxRow = delEntity.m_IdxRow;
		m_Entitys[movEntityIdx].m_IdxCol = delEntity.m_IdxCol;
	}

	size_t lastEntityIdx = m_Entitys.size() - 1;

	if (delEntityIdx != lastEntityIdx)
	{
		m_Entitys[delEntityIdx] = std::move(m_Entitys[lastEntityIdx]);
		m_LookupTable[m_Entitys[delEntityIdx].m_IdxLookup] = delEntityIdx;
	}

	m_Entitys.pop_back();
}

const Entity* ECSSystem::GetEntity(size_t lookupIdx)
{
	return &m_Entitys[FindEntity(lookupIdx)];
}

size_t ECSSystem::FindEntity(size_t lookupIdx)
{
	_ASEERTION_NULCHK(lookupIdx < m_LookupTable.size(), "lookupIdx Out of bounds");
	_ASEERTION_NULCHK(m_LookupTable[lookupIdx] != _HashNotInitialize, "lookupIdx invalid");

	size_t entityIdx = m_LookupTable[lookupIdx];

	_ASEERTION_NULCHK(entityIdx < m_Entitys.size(), "entityIdx Out of bounds");
	_ASEERTION_NULCHK(m_Entitys[entityIdx].m_IdxLookup == lookupIdx, "LookupTable mismatch");

	return m_LookupTable[lookupIdx];
}
