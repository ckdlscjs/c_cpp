
//#include "CameraSystem.h"
//#include "Camera.h"
//#include "InputSystem.h"
//CameraSystem::CameraSystem()
//{
//
//}
//
//BaseCamera* CameraSystem::GetCamera(size_t idx)
//{
//	return  m_Cameras[idx];
//}
//
//void CameraSystem::AddCamera(BaseCamera* camera)
//{
//	_ASEERTION_NULCHK(camera, "Camera is NULL");
//	m_Cameras[m_lCameraID++] = camera;
//}
//
//void CameraSystem::Init()
//{
//	m_lCameraID = 0;
//	
//	//마우스 이동에따른 카메라의 변환
//	m_IdxCallbacks[E_InputEvent::MOUSE_MOVE].push_back(_InputSystem.AddListner(E_InputEvent::MOUSE_MOVE, [this](const InputEvent& event)->void
//		{
//			for (auto iter = m_Cameras.begin(); iter != m_Cameras.end(); iter++)
//			{
//				//넘겨받은 deltaX,Y를 이용해 카메라의 정보를갱신
//				iter->second->UpdateEulerRotate(event.mouseDeltaX, event.mouseDeltaY, 0.1f);
//			}
//		}
//	));
//	/*
//	m_IdxCallbacks[E_InputEvent::KEY_DOWN].push_back(_InputSystem.AddListner(E_InputEvent::KEY_DOWN, [this](const InputEvent& event)->void
//		{
//
//		}
//	));
//	
//	m_IdxCallbacks[InputEventType::KEY_PRESSED].push_back(_InputSystem.AddListner(InputEventType::KEY_PRESSED, [this](const InputEvent& event)->void
//		{
//			if (VK_MASK.find(event.keyCode) == VK_MASK.end()) return;
//			m_lControlMask |= (1LL << VK_MASK[event.keyCode]);
//		}
//	));
//	
//	m_IdxCallbacks[E_InputEvent::KEY_UP].push_back(_InputSystem.AddListner(E_InputEvent::KEY_UP, [this](const InputEvent& event)->void
//		{
//
//		}
//	));
//	*/
//}
//
//void CameraSystem::Frame(float deltaTime)
//{
//	std::cout << "Frame : " << "CameraSystem" << " Class" << '\n';
//	
//	float delta_MovX = 0.0f;
//	if (_InputSystem.GetKeyState('A')) delta_MovX += -1.0f;				//left
//	if (_InputSystem.GetKeyState('D')) delta_MovX += +1.0f;				//right
//
//	float delta_MovY = 0.0f;
//	if (_InputSystem.GetKeyState('Q')) delta_MovY += -1.0f;				//down
//	if (_InputSystem.GetKeyState('E')) delta_MovY += +1.0f;				//up
//
//	float delta_MovZ = 0.0f;
//	if (_InputSystem.GetKeyState('W')) delta_MovZ += +1.0f;				//forward
//	if (_InputSystem.GetKeyState('S')) delta_MovZ += -1.0f;				//back
//	for (auto iter = m_Cameras.begin(); iter != m_Cameras.end(); iter++)
//	{
//		Vector3 vMove(delta_MovX, delta_MovY, delta_MovZ);
//		vMove *= iter->second->GetMoveSpeed() * deltaTime;
//		iter->second->MovePosition(vMove);
//	}
//}
//
//CameraSystem::~CameraSystem()
//{
//	for (auto iter = m_Cameras.begin(); iter != m_Cameras.end();)
//	{
//		delete iter->second;
//		iter = m_Cameras.erase(iter);
//	}
//
//	for (const auto& category : m_IdxCallbacks)
//		for (const auto& iter : category.second)
//			_InputSystem.RemoveListner(category.first, iter);
//}


#include "CameraSystem.h"
#include "Components.h"
#include "ECSSystem.h"

CameraSystem::CameraSystem()
{

}
CameraSystem::~CameraSystem()
{

}

void CameraSystem::Init()
{
	m_Key = _ECSSystem.GetArchetypeKey<C_Transform, C_Camera>();
}

void CameraSystem::Frame(float deltaTime)
{
	std::cout << "Frame : " << "CameraSystem" << " Class" << '\n';
	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Camera>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);

	//Input이 있는 인덱스를 기록하고 뒤로 밀집시킨다
	for (auto& archetype : queries)
	{
		size_t st_row = archetype->m_transfer_row;
		size_t st_col = archetype->m_transfer_col;
		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
		{
			auto& transforms = archetype->GetComponents<C_Transform>(row);
			auto& cameras = archetype->GetComponents<C_Camera>(row);

			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
			{
				const Vector3& scale = transforms[col].vScale;
				const Quarternion& rotate = transforms[col].qRotate;
				const Vector3& position = transforms[col].vPosition;
				cameras[col].m_MatWorld = GetMat_World(scale, rotate, position);
				cameras[col].m_MatView = GetMat_View(position, rotate);
				cameras[col].m_MatProj = GetMat_Perspective(cameras[col].fScreenWidth / cameras[col].fScreenHeight, cameras[col].fFov, cameras[col].fNear, cameras[col].fFar);
				//ortho leftup
				cameras[col].m_MatOrtho = GetMat_Orthographic_OffCenter(0.0f, cameras[col].fScreenWidth, 0.0f, cameras[col].fScreenHeight, cameras[col].fNear, cameras[col].fFar);
			}
		}
	}
}

//void CameraSystem::SetPosition(int chunkIdx, const Vector3& position)
//{
//	std::vector<C_Transform>& chunks = _ECSSystem.GetComponents<C_Transform>(m_Key);
//	_ASEERTION_NULCHK(chunkIdx < chunks.size(), "idx out of bound");
//	chunks[chunkIdx].m_vPosition = position;
//}
//
//void CameraSystem::SetFOV(int chunkIdx, float FOV)
//{
//	std::vector<C_Camera>& chunks = _ECSSystem.GetComponents<C_Camera>(m_Key);
//	_ASEERTION_NULCHK(chunkIdx < chunks.size(), "idx out of bound");
//	chunks[chunkIdx].m_fFov = FOV;
//}
//
//void CameraSystem::SetScreenWidthHeight(int chunkIdx, float width, float height)
//{
//	std::vector<C_Camera>& chunks = _ECSSystem.GetComponents<C_Camera>(m_Key);
//	_ASEERTION_NULCHK(chunkIdx < chunks.size(), "idx out of bound");
//	chunks[chunkIdx].m_fScreenWidth = width;
//	chunks[chunkIdx].m_fScreenHeight = height;
//}
//
//void CameraSystem::SetClipPlanes(int chunkIdx, float nearZ, float farZ)
//{
//	std::vector<C_Camera>& chunks = _ECSSystem.GetComponents<C_Camera>(m_Key);
//	_ASEERTION_NULCHK(chunkIdx < chunks.size(), "idx out of bound");
//	chunks[chunkIdx].m_fNear = nearZ;
//	chunks[chunkIdx].m_fFar = farZ;
//}
//
//const Vector3& CameraSystem::GetPosition(int chunkIdx)
//{
//	std::vector<C_Transform>& chunks = _ECSSystem.GetComponents<C_Transform>(m_Key);
//	_ASEERTION_NULCHK(chunkIdx < chunks.size(), "idx out of bound");
//	return chunks[chunkIdx].m_vPosition;
//}
//
//const Matrix4x4& CameraSystem::GetViewMatrix(int chunkIdx)
//{
//	std::vector<C_Transform>& chunks = _ECSSystem.GetComponents<C_Transform>(m_Key);
//	_ASEERTION_NULCHK(chunkIdx < chunks.size(), "idx out of bound");
//	return GetMat_View(chunks[chunkIdx].m_vPosition, chunks[chunkIdx].m_qRotate);
//}
//
//const Matrix4x4& CameraSystem::GetProjMatrix(int chunkIdx)
//{
//	std::vector<C_Camera>& chunks = _ECSSystem.GetComponents<C_Camera>(m_Key);
//	_ASEERTION_NULCHK(chunkIdx < chunks.size(), "idx out of bound");
//	return GetMat_Perspective(chunks[chunkIdx].m_fScreenWidth, chunks[chunkIdx].m_fScreenHeight, chunks[chunkIdx].m_fFov, chunks[chunkIdx].m_fNear, chunks[chunkIdx].m_fFar);
//}
//
//const Matrix4x4& CameraSystem::GetOrthoMatrix_CT(int chunkIdx)
//{
//	std::vector<C_Camera>& chunks = _ECSSystem.GetComponents<C_Camera>(m_Key);
//	_ASEERTION_NULCHK(chunkIdx < chunks.size(), "idx out of bound");
//	return GetMat_Orthographic(chunks[chunkIdx].m_fScreenWidth, chunks[chunkIdx].m_fScreenHeight, chunks[chunkIdx].m_fNear, chunks[chunkIdx].m_fFar);
//}
//
//const Matrix4x4& CameraSystem::GetOrthoMatrix_LT(int chunkIdx)
//{
//	std::vector<C_Camera>& chunks = _ECSSystem.GetComponents<C_Camera>(m_Key);
//	_ASEERTION_NULCHK(chunkIdx < chunks.size(), "idx out of bound");
//	return GetMat_Orthographic_OffCenter(0.0f, chunks[chunkIdx].m_fScreenWidth, 0.0f, chunks[chunkIdx].m_fScreenHeight, chunks[chunkIdx].m_fNear, chunks[chunkIdx].m_fFar);
//}

