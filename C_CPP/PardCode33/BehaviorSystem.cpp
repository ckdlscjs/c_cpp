#include "BehaviorSystem.h"
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
	float sensitivity = 0.1f;
	Vector2 mouseDelta = _InputSystem.GetMouseDelta();
	float dt_Pitch = mouseDelta.GetY() * sensitivity;	//sensitivity
	float dt_Yaw = mouseDelta.GetX() * sensitivity;

	//NoInputBlock
	{

	}

	//InputBlock
	{
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

					//MouseRotate
					if (_InputSystem.IsPressed_RBTN() && inputs[col].bVKMask[VK_MOUSE_MOVE])
					{
						Vector3 eulerRotate = transforms[col].qRotate.ToRotate();
						//std::cout << eulerRotate.GetX() << ' ' << eulerRotate.GetY() << '\n';
						float rotPitch = eulerRotate.GetX() + dt_Pitch;
						float rotYaw = eulerRotate.GetY() + dt_Yaw;
						//Clamp(rotPitch, -89.9f, 89.9f);	//need?
						transforms[col].qRotate.SetFromRotate(rotPitch, rotYaw, 0.0f);
					}
				}
				st_col = 0;
			}
		}
	}
}
