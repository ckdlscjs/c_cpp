#include "BehaviorSystem.h"
#include "Components.h"
#include "ECSSystem.h"
#include "InputSystem.h"
BehaviorSystem::BehaviorSystem()
{
}

BehaviorSystem::~BehaviorSystem()
{
}

void BehaviorSystem::Init()
{
}

void BehaviorSystem::Frame(float deltatime)
{
	std::cout << "Frame : " << "BehaviorSystem" << " Class" << '\n';
	
	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Input, C_Transform, C_Behavior>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
	const E_InputEvent* keysState = _InputSystem.GetKeysState();

	//Input이 있는 인덱스를 기록하고 뒤로 밀집시킨다
	for (auto& archetype : queries)
	{
		size_t st_row = archetype->m_transfer_row;
		size_t st_col = archetype->m_transfer_col;
		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
		{
			auto& inputs = archetype->GetComponents<C_Input>(row);
			auto& transforms = archetype->GetComponents<C_Transform>(row);
			auto& behaviors = archetype->GetComponents<C_Behavior>(row);
			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
			{
				//Movement
				Vector3 moveDir(0.0f, 0.0f, 0.0f);
				for (UINT idx = 1; idx < E_Behavior::COUNT; idx++)
				{
					auto vk_key = behaviors[col].BehaviorMap[idx];
					if ((keysState[vk_key] == E_InputEvent::KEY_DOWN || keysState[vk_key] == E_InputEvent::KEY_PRESSED) && inputs[col].bVKMask[vk_key])
						moveDir += MoveDirs[idx];
				}
				transforms[col].vPosition += (transforms[col].qRotate * moveDir.Normalize()) * 100.0f * deltatime;	//speed * dt


				//Rotate
				if (inputs[col].bVKMask[255])
				{
					Vector2 mouseDelta = _InputSystem.GetMouseDelta();
					float yaw = mouseDelta.GetX() * 0.01f;	//sensitivity
					float pitch = mouseDelta.GetY() * 0.01f;
					transforms[col].qRotate *= Vector3(pitch, yaw, 0.0f);
					//std::cout << transforms[col].qRotate.ToRotate().GetX() << '\n';
				}
			}
		}
	}
}
