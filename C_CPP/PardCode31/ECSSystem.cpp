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
	m_LookupTable[lookupIdx] = -1;	//�ش� ����� ���
	//������Ų Lookup�� ������ų Lookup�� ���ٸ� ûũ�� ���� �����Ȱ�
	if (movedLookupIdx == lookupIdx)
	{
		//������ ��ƼƼ�� ������ ��ƼƼ�� �ڸ��� swap and pop�Ѵ�
		m_LookupTable[m_Entitys.back().m_IdxLookup] = delEntityIdx;	//�������� ����Ű�� ����� ���ҿ�ƼƼ������
		m_Entitys[delEntityIdx] = std::move(m_Entitys.back());		//�����ڸ��� ��������ƼƼ�� �̵�
		m_Entitys.pop_back();
		return;
	}
	//�Ű��� ��ƼƼ�� row, col�� ������ row,col�� ���Ž�Ų��
	size_t movEntityIdx = FindEntity(movedLookupIdx);
	m_Entitys[movEntityIdx].m_IdxRow = delEntity.m_IdxRow;
	m_Entitys[movEntityIdx].m_IdxCol = delEntity.m_IdxCol;

	//���� ���� ��ƼƼ�� ������ �ڸ��� �ű��(swap and pop)
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
