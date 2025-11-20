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
	float pitch = mouseDelta.GetY() * sensitivity;	//sensitivity
	float yaw = mouseDelta.GetX() * sensitivity;

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
					if (inputs[col].bVKMask[255])
					{
						//roll * pitch * yaw를 따른다
						Quarternion qPitch(transforms[col].qRotate.GetRightAxis(), pitch);
						Quarternion qYaw(Vector3(0.0f, 1.0f, 0.0f), yaw);
						transforms[col].qRotate = qPitch * qYaw * transforms[col].qRotate;	//qvq'의 순서상 회전시키려는 사원수(v에해당)가 가장 우측에온다
					}
				}
			}
			archetype->m_transfer_row = 0;
			archetype->m_transfer_col = 0;
		}
	}
	
}
