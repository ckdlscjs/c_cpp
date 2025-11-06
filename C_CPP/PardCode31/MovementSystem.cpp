#include "MovementSystem.h"
#include "Components.h"
#include "ECSSystem.h"
#include "InputSystem.h"
MovementSystem::MovementSystem()
{
}
MovementSystem::~MovementSystem()
{
}

void MovementSystem::Init()
{
}

void MovementSystem::Frame(float deltatime)
{
	std::cout << "Frame : " << "MovementSystem" << " Class" << '\n';
	
	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Input, C_Transform, C_Movement>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
	//Input이 있는 인덱스를 기록하고 뒤로 밀집시킨다
	for (auto& archetype : queries)
	{
		size_t st_row = archetype->m_transfer_row;
		size_t st_col = archetype->m_transfer_col;
		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
		{
			auto& inputs = archetype->GetComponents<C_Input>(row);
			auto& transforms = archetype->GetComponents<C_Transform>(row);
			auto& movements = archetype->GetComponents<C_Movement>(row);
			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
			{

			}
		}
	}
}
