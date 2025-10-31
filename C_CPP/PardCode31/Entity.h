#pragma once
#include "CommonHeader.h"
class Entity
{
public:
	ArchetypeKey m_Key;
	size_t m_IdxRow;
	size_t m_IdxCol;
	size_t m_IdxLookup;
};