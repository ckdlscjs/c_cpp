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
	m_vRotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fSpeedRotate = 100.0f;
	m_vTranslation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fSpeedMove = 100.0f;
	m_lControlMask = 0;
	int maskNum = 0;
	VK_MASK[VK_LEFT] = maskNum++;			//left
	VK_MASK[VK_RIGHT] = maskNum++;			//right
	VK_MASK[VK_UP] = maskNum++;				//up
	VK_MASK[VK_DOWN] = maskNum++;			//right
	VK_MASK[VK_PRIOR] = maskNum++;			//pageup
	VK_MASK[VK_NEXT] = maskNum++;			//pagedown
	VK_MASK[VK_OEM_PLUS] = maskNum++;		//+
	VK_MASK[VK_OEM_MINUS] = maskNum++;		//-

	m_IdxCallbacks[InputEventType::KEY_DOWN].push_back(_InputSystem.AddListner(InputEventType::KEY_DOWN, [this](const InputEvent& event)->void
		{
			if (VK_MASK.find(event.keyCode) == VK_MASK.end()) return;
			m_lControlMask |= (1LL << VK_MASK[event.keyCode]);
		}
	));

	m_IdxCallbacks[InputEventType::KEY_PRESSED].push_back(_InputSystem.AddListner(InputEventType::KEY_PRESSED, [this](const InputEvent& event)->void
		{
			if (VK_MASK.find(event.keyCode) == VK_MASK.end()) return;
			m_lControlMask |= (1LL << VK_MASK[event.keyCode]);
		}
	));

	m_IdxCallbacks[InputEventType::KEY_UP].push_back(_InputSystem.AddListner(InputEventType::KEY_UP, [this](const InputEvent& event)->void
		{
			if (VK_MASK.find(event.keyCode) == VK_MASK.end()) return;
			m_lControlMask &= ~(1LL << VK_MASK[event.keyCode]);
		}
	));
}

void TempObj::Frame(float deltaTime)
{
	float deltaY = 0.0f;
	if (m_lControlMask & (1LL << VK_MASK[VK_LEFT])) deltaY += -m_fSpeedRotate * deltaTime;			//Yaw-
	if (m_lControlMask & (1LL << VK_MASK[VK_RIGHT])) deltaY += +m_fSpeedRotate * deltaTime;			//Yaw+
	m_vRotate.y += deltaY;

	float deltaX = 0.0f;
	if (m_lControlMask & (1LL << VK_MASK[VK_UP])) deltaX += m_fSpeedRotate * deltaTime;				//Pitch+
	if (m_lControlMask & (1LL << VK_MASK[VK_DOWN])) deltaX += -m_fSpeedRotate * deltaTime;			//Pitch-
	m_vRotate.x += deltaX;

	float deltaZ = 0.0f;
	if (m_lControlMask & (1LL << VK_MASK[VK_PRIOR])) deltaZ += m_fSpeedRotate * deltaTime;			//Roll+
	if (m_lControlMask & (1LL << VK_MASK[VK_NEXT])) deltaZ += -m_fSpeedRotate * deltaTime;			//Roll-
	m_vRotate.z += deltaZ;

	float deltaScale = 0.0f;
	if (m_lControlMask & (1LL << VK_MASK[VK_OEM_PLUS])) deltaScale += m_fSpeedRotate * deltaTime;	//Scale+
	if (m_lControlMask & (1LL << VK_MASK[VK_OEM_MINUS])) deltaScale += -m_fSpeedRotate * deltaTime;	//Scale-
	m_vScale = XMFLOAT3(m_vScale.x + deltaScale, m_vScale.y + deltaScale, m_vScale.z + deltaScale);
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
