#include "CameraSystem.h"
#include "Camera.h"
#include "InputSystem.h"

CameraSystem::CameraSystem()
{

}

BaseCamera* CameraSystem::GetCamera(size_t idx)
{
	return  m_Cameras[idx];
}

void CameraSystem::AddCamera(BaseCamera* camera)
{
	_ASEERTION_NULCHK(camera, "Camera is NULL");
	m_Cameras[m_lCameraID++] = camera;
}

void CameraSystem::Init()
{
	m_lCameraID = 0;
	
	//마우스 이동에따른 카메라의 변환
	m_IdxCallbacks[E_InputEvent::MOUSE_MOVE].push_back(_InputSystem.AddListner(E_InputEvent::MOUSE_MOVE, [this](const InputEvent& event)->void
		{
			for (auto iter = m_Cameras.begin(); iter != m_Cameras.end(); iter++)
			{
				//넘겨받은 deltaX,Y를 이용해 카메라의 정보를갱신
				iter->second->UpdateEulerRotate(event.mouseDeltaX, event.mouseDeltaY, 0.1f);
			}
		}
	));
	/*
	m_IdxCallbacks[E_InputEvent::KEY_DOWN].push_back(_InputSystem.AddListner(E_InputEvent::KEY_DOWN, [this](const InputEvent& event)->void
		{

		}
	));
	
	m_IdxCallbacks[InputEventType::KEY_PRESSED].push_back(_InputSystem.AddListner(InputEventType::KEY_PRESSED, [this](const InputEvent& event)->void
		{
			if (VK_MASK.find(event.keyCode) == VK_MASK.end()) return;
			m_lControlMask |= (1LL << VK_MASK[event.keyCode]);
		}
	));
	
	m_IdxCallbacks[E_InputEvent::KEY_UP].push_back(_InputSystem.AddListner(E_InputEvent::KEY_UP, [this](const InputEvent& event)->void
		{

		}
	));
	*/
}

void CameraSystem::Frame(float deltaTime)
{
	std::cout << "Frame : " << "CameraSystem" << " Class" << '\n';
	
	float delta_MovX = 0.0f;
	if (_InputSystem.GetKeyState('A')) delta_MovX += -1.0f;				//left
	if (_InputSystem.GetKeyState('D')) delta_MovX += +1.0f;				//right

	float delta_MovY = 0.0f;
	if (_InputSystem.GetKeyState('Q')) delta_MovY += -1.0f;				//down
	if (_InputSystem.GetKeyState('E')) delta_MovY += +1.0f;				//up

	float delta_MovZ = 0.0f;
	if (_InputSystem.GetKeyState('W')) delta_MovZ += +1.0f;				//forward
	if (_InputSystem.GetKeyState('S')) delta_MovZ += -1.0f;				//back
	for (auto iter = m_Cameras.begin(); iter != m_Cameras.end(); iter++)
	{
		Vector3 vMove(delta_MovX, delta_MovY, delta_MovZ);
		vMove *= iter->second->GetMoveSpeed() * deltaTime;
		iter->second->MovePosition(vMove);
	}
}

CameraSystem::~CameraSystem()
{
	for (auto iter = m_Cameras.begin(); iter != m_Cameras.end();)
	{
		delete iter->second;
		iter = m_Cameras.erase(iter);
	}

	for (const auto& category : m_IdxCallbacks)
		for (const auto& iter : category.second)
			_InputSystem.RemoveListner(category.first, iter);
}