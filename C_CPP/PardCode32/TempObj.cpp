#include "TempObj.h"
#include "InputSystem.h"

TempObj::TempObj()
{
	std::cout << "Initialize : " << "TempObj" << " Class"  << '\n';
	Init();
}

TempObj::~TempObj()
{
	std::cout << "Release : " << "TempObj" << " Class"  << '\n';
	Release();
}

void TempObj::Init()
{
	m_vScale = Vector3(1.0f, 1.0f, 1.0f);
	m_fSpeedScale = 100.0f;
	m_vRotate = Vector3(0.0f, 0.0f, 0.0f);
	m_fSpeedRotate = 100.0f;
	m_vPosition = Vector3(0.0f, 0.0f, 0.0f);
	m_fSpeedMove = 100.0f;
	/*
	m_IdxCallbacks[E_InputEvent::KEY_DOWN].push_back(_InputSystem.AddListner(E_InputEvent::KEY_DOWN, [this](const InputEvent& event)->void
		{ 
			
		}
	));
	
	m_IdxCallbacks[E_InputEvent::KEY_PRESSED].push_back(_InputSystem.AddListner(E_InputEvent::KEY_PRESSED, [this](const InputEvent& event)->void
		{
			
		}
	));
	
	m_IdxCallbacks[E_InputEvent::KEY_UP].push_back(_InputSystem.AddListner(E_InputEvent::KEY_UP, [this](const InputEvent& event)->void
		{
			
		}
	));

	
	m_IdxCallbacks[InputEventType::MOUSE_MOVE].push_back(_InputSystem.AddListner(InputEventType::MOUSE_MOVE, [this](const InputEvent& event)->void
		{
			std::cout << "MousePosVal : (" << event.mouseX << ", " << event.mouseY << ")" << '\n';
			std::cout << "MouseDeltaVal : (" << event.mouseDeltaX << ", " << event.mouseDeltaY << ")" << '\n';
		}
	));
	m_IdxCallbacks[InputEventType::MOUSE_L_DOWN].push_back(_InputSystem.AddListner(InputEventType::MOUSE_L_DOWN, [this](const InputEvent& event)->void
		{
		}
	));

	m_IdxCallbacks[InputEventType::MOUSE_L_UP].push_back(_InputSystem.AddListner(InputEventType::MOUSE_L_UP, [this](const InputEvent& event)->void
		{
		}
	));

	m_IdxCallbacks[InputEventType::MOUSE_R_DOWN].push_back(_InputSystem.AddListner(InputEventType::MOUSE_R_DOWN, [this](const InputEvent& event)->void
		{
		}
	));
	m_IdxCallbacks[InputEventType::MOUSE_R_UP].push_back(_InputSystem.AddListner(InputEventType::MOUSE_R_UP, [this](const InputEvent& event)->void
		{
		}
	));
	*/
}

void TempObj::Frame(float deltaTime)
{
	//float delta_RotY = 0.0f;
	//if (CheckMask(VK_LEFT)) delta_RotY += -m_fSpeedRotate * deltaTime;			//Yaw-
	//if (CheckMask(VK_RIGHT)) delta_RotY += +m_fSpeedRotate * deltaTime;			//Yaw+

	//float delta_RotX = 0.0f;
	//if (CheckMask(VK_UP)) delta_RotX += m_fSpeedRotate * deltaTime;				//Pitch+
	//if (CheckMask(VK_DOWN)) delta_RotX += -m_fSpeedRotate * deltaTime;			//Pitch-

	//float delta_RotZ = 0.0f;
	//if (CheckMask(VK_PRIOR)) delta_RotZ += m_fSpeedRotate * deltaTime;			//Roll+
	//if (CheckMask(VK_NEXT)) delta_RotZ += -m_fSpeedRotate * deltaTime;			//Roll-
	//m_vRotate += Vector3(delta_RotX, delta_RotY, delta_RotZ);

	//float delta_Scale = 0.0f;
	//if (CheckMask(VK_OEM_PLUS)) delta_Scale += m_fSpeedScale * deltaTime;		//Scale+
	//if (CheckMask(VK_OEM_MINUS)) delta_Scale += -m_fSpeedScale * deltaTime;		//Scale-
	//m_vScale += Vector3(delta_Scale, delta_Scale, delta_Scale);
	//
	//float delta_MovX = 0.0f;
	//if (CheckMask('A')) delta_MovX += -m_fSpeedMove * deltaTime;				//left
	//if (CheckMask('D')) delta_MovX += +m_fSpeedMove * deltaTime;				//right

	//float delta_MovY = 0.0f;
	//if (CheckMask('Q')) delta_MovY += -m_fSpeedMove * deltaTime;				//down
	//if (CheckMask('E')) delta_MovY += +m_fSpeedMove * deltaTime;				//up

	//float delta_MovZ = 0.0f;
	//if (CheckMask('W')) delta_MovZ += +m_fSpeedMove * deltaTime;				//forward
	//if (CheckMask('S')) delta_MovZ += -m_fSpeedMove * deltaTime;				//back
	//m_vPosition += Vector3(delta_MovX, delta_MovY, delta_MovZ);
}

void TempObj::Render()
{
}

void TempObj::Release()
{
	/*for (const auto& category : m_IdxCallbacks)
		for (const auto& iter : category.second)
			_InputSystem.RemoveListner(category.first, iter);*/
}