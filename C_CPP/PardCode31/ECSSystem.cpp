#include "ECSSystem.h"
//#include "Archetype.h"
#include "Components.h"
#include "Entity.h"

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
	auto key1 = CreateArchetype<Component_Transform, Component_Input>();
}
