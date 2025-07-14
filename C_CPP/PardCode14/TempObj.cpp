#include "TempObj.h"
#include "InputSystem.h"

TempObj::TempObj()
{
	std::cout << "TempObj" << " Class" << " 생성자 호출" << '\n';
	Init();
}

TempObj::~TempObj()
{
	std::cout << "TempObj" << " Class" << " 소멸자 호출" << '\n';
	Release();
}

void TempObj::Init()
{
	m_vScale = XMFLOAT3(200.0f, 200.0f, 200.0f);
	m_fSpeedScale = 100.0f;
	m_vRotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fSpeedRotate = 100.0f;
	m_vTranslation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fSpeedMove = 100.0f;
	m_lVKMask = 0;
	int maskNum = 0;
	VK_MASK[VK_LEFT] = maskNum++;			//left
	VK_MASK[VK_RIGHT] = maskNum++;			//right
	VK_MASK[VK_UP] = maskNum++;				//up
	VK_MASK[VK_DOWN] = maskNum++;			//right
	VK_MASK[VK_PRIOR] = maskNum++;			//pageup
	VK_MASK[VK_NEXT] = maskNum++;			//pagedown
	VK_MASK[VK_OEM_PLUS] = maskNum++;		//+
	VK_MASK[VK_OEM_MINUS] = maskNum++;		//-
	VK_MASK['W'] = maskNum++;				//W
	VK_MASK['A'] = maskNum++;				//A
	VK_MASK['S'] = maskNum++;				//S
	VK_MASK['D'] = maskNum++;				//D
	VK_MASK['Q'] = maskNum++;				//Q
	VK_MASK['E'] = maskNum++;				//E
	VK_MASK[VK_LBUTTON] = maskNum++;		//MouseLeft
	VK_MASK[VK_RBUTTON] = maskNum++;		//MouseRight
	VK_MASK[VK_MBUTTON] = maskNum++;		//MouseMiddle


	m_IdxCallbacks[InputEventType::KEY_DOWN].push_back(_InputSystem.AddListner(InputEventType::KEY_DOWN, [this](const InputEvent& event)->void
		{
			if (VK_MASK.find(event.keyCode) == VK_MASK.end()) return;
			m_lVKMask |= (1LL << VK_MASK[event.keyCode]);
		}
	));
	/*
	m_IdxCallbacks[InputEventType::KEY_PRESSED].push_back(_InputSystem.AddListner(InputEventType::KEY_PRESSED, [this](const InputEvent& event)->void
		{
			if (VK_MASK.find(event.keyCode) == VK_MASK.end()) return;
			m_lControlMask |= (1LL << VK_MASK[event.keyCode]);
		}
	));
	*/
	m_IdxCallbacks[InputEventType::KEY_UP].push_back(_InputSystem.AddListner(InputEventType::KEY_UP, [this](const InputEvent& event)->void
		{
			if (VK_MASK.find(event.keyCode) == VK_MASK.end()) return;
			m_lVKMask &= ~(1LL << VK_MASK[event.keyCode]);
		}
	));

	m_IdxCallbacks[InputEventType::MOUSE_MOVE].push_back(_InputSystem.AddListner(InputEventType::MOUSE_MOVE, [this](const InputEvent& event)->void
		{
			std::cout << "MousePosVal : (" << event.mouseX << ", " << event.mouseY << ")" << '\n';
			std::cout << "MouseDeltaVal : (" << event.mouseDeltaX << ", " << event.mouseDeltaY << ")" << '\n';
		}
	));

	/*NEED?
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
	float delta_RotY = 0.0f;
	if (m_lVKMask & (1LL << VK_MASK[VK_LEFT])) delta_RotY += -m_fSpeedRotate * deltaTime;			//Yaw-
	if (m_lVKMask & (1LL << VK_MASK[VK_RIGHT])) delta_RotY += +m_fSpeedRotate * deltaTime;			//Yaw+
	m_vRotate.y += delta_RotY;

	float delta_RotX = 0.0f;
	if (m_lVKMask & (1LL << VK_MASK[VK_UP])) delta_RotX += m_fSpeedRotate * deltaTime;				//Pitch+
	if (m_lVKMask & (1LL << VK_MASK[VK_DOWN])) delta_RotX += -m_fSpeedRotate * deltaTime;			//Pitch-
	m_vRotate.x += delta_RotX;

	float delta_RotZ = 0.0f;
	if (m_lVKMask & (1LL << VK_MASK[VK_PRIOR])) delta_RotZ += m_fSpeedRotate * deltaTime;			//Roll+
	if (m_lVKMask & (1LL << VK_MASK[VK_NEXT])) delta_RotZ += -m_fSpeedRotate * deltaTime;			//Roll-
	m_vRotate.z += delta_RotZ;

	float delta_Scale = 0.0f;
	if (m_lVKMask & (1LL << VK_MASK[VK_OEM_PLUS])) delta_Scale += m_fSpeedScale * deltaTime;		//Scale+
	if (m_lVKMask & (1LL << VK_MASK[VK_OEM_MINUS])) delta_Scale += -m_fSpeedScale * deltaTime;		//Scale-
	m_vScale = XMFLOAT3(m_vScale.x + delta_Scale, m_vScale.y + delta_Scale, m_vScale.z + delta_Scale);

	float delta_MovX = 0.0f;
	if (m_lVKMask & (1LL << VK_MASK['A'])) delta_MovX += -m_fSpeedMove * deltaTime;				//left
	if (m_lVKMask & (1LL << VK_MASK['D'])) delta_MovX += +m_fSpeedMove * deltaTime;				//right
	m_vTranslation.x += delta_MovX;

	float delta_MovY = 0.0f;
	if (m_lVKMask & (1LL << VK_MASK['Q'])) delta_MovY += -m_fSpeedMove * deltaTime;				//down
	if (m_lVKMask & (1LL << VK_MASK['E'])) delta_MovY += +m_fSpeedMove * deltaTime;				//up
	m_vTranslation.y += delta_MovY;

	float delta_MovZ = 0.0f;
	if (m_lVKMask & (1LL << VK_MASK['W'])) delta_MovZ += +m_fSpeedMove * deltaTime;				//forward
	if (m_lVKMask & (1LL << VK_MASK['S'])) delta_MovZ += -m_fSpeedMove * deltaTime;				//back
	m_vTranslation.z += delta_MovZ;
}

void TempObj::Render()
{
}

void TempObj::Release()
{
	for (const auto& category : m_IdxCallbacks)
		for (const auto& iter : category.second)
			_InputSystem.RemoveListner(category.first, iter);
}
