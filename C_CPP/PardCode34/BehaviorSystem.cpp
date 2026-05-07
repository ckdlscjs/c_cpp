#include "BehaviorSystem.h"
#include "ECSSystem.h"
#include "InputSystem.h"
#include "CameraSystem.h"

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
	if(g_fTime_Log >= 1.0f)
		std::cout << "Frame : " << "BehaviorSystem" << " Class" << '\n';
	float sensitivity = 0.1f;
	Vector2 mouseDelta = _InputSystem.GetMouseDelta();
	float dt_Pitch = mouseDelta.GetY() * sensitivity;	//sensitivity
	float dt_Yaw = mouseDelta.GetX() * sensitivity;


	//메인카메라
	size_t lookup_maincam = _CameraSystem.lookup_maincam;
	//const auto& c_cam_main = _ECSSystem.GetComponent<C_Camera>(lookup_maincam);
	//const auto& c_cam_proj = _ECSSystem.GetComponent<C_Projection>(lookup_maincam);
	//const auto& c_cam_ortho = _ECSSystem.GetComponent<C_Orthographic>(lookup_maincam);
	//const Matrix4x4& cam_matWorld = c_cam_main.matWorld;
	//const Matrix4x4& cam_matView = c_cam_main.matView;
	//const Matrix4x4& cam_matProj = c_cam_proj.matProj;
	//const Matrix4x4& cam_matOrhto = c_cam_ortho.matOrtho;
	const Vector3& cam_pos = _ECSSystem.GetComponent<C_Transform>(lookup_maincam).vPosition;

	//NoInputBlock
	{
		ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, T_Render_Billboard>();
		std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
		const E_InputEvent* keysState = _InputSystem.GetKeysState();

		//Input이 있는 인덱스를 기록하고 뒤로 밀집시킨다
		for (auto& archetype : queries)
		{
			size_t st_row = archetype->m_transfer_row;
			size_t st_col = archetype->m_transfer_col;
			for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
			{
				auto& transforms = archetype->GetComponents<C_Transform>(row);
				for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
				{
					/*
						float dz = c_cam_transform.vPosition.GetZ() - position.GetZ();
						float dx = c_cam_transform.vPosition.GetX() - position.GetX();
						double angle = _RADTODEG(atan2f(dx, dz)) + 180.0f;
					*/
					//yaw빌보드 구현, +z(화면안쪽) 이므로 물체-카메라로 역방향으로 회전해야 같은yaw로 회전해 빌보드로 보이게된다
					const auto& pos = transforms[col].vPosition;

					Quaternion qRot((pos - cam_pos).Normalize());
					transforms[col].qRotate = Quaternion(0.0f, qRot.ToRotate().GetY(), 0.0f);
				}
				st_col = 0;
			}
		}
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
