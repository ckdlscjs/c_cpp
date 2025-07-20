#include "InputSystem.h"


InputSystem::InputSystem()
{
	std::cout << "InputSystem" << " Class" << " 생성자 호출" << '\n';
}

InputSystem::~InputSystem()
{
	std::cout << "InputSystem" << " Class" << " 소멸자 호출" << '\n';
}

size_t InputSystem::AddListner(InputEventType type, EventCallBack callback)
{
	if (!callback) return 0; //유효한 콜백이 아니면 (0, 오류)
	m_Listners[type][m_CallbackID] = callback;
	return m_CallbackID++;
}

void InputSystem::RemoveListner(InputEventType type, size_t id)
{
	if (m_Listners[type].empty()) return;
	m_Listners[type].erase(id);
}

void InputSystem::OnKeyDown(unsigned char VK_KEY)
{
	if (VK_KEY < 0 || VK_KEY > 255) return;
	if (m_OldKeystate[VK_KEY]) return; 
	m_CurKeystate[VK_KEY] = true;
	InputEvent event;
	event.type = InputEventType::KEY_DOWN;
	event.keyCode = VK_KEY;
	Notify(event);
}

void InputSystem::OnKeyPressed(unsigned char VK_KEY)
{
	if (VK_KEY < 0 || VK_KEY > 255) return;
	InputEvent event;
	event.type = InputEventType::KEY_PRESSED;
	event.keyCode = VK_KEY;
	Notify(event);
}

void InputSystem::OnKeyUp(unsigned char VK_KEY)
{
	if (VK_KEY < 0 || VK_KEY > 255) return;
	if (!m_OldKeystate[VK_KEY]) return; 
	m_CurKeystate[VK_KEY] = false;
	InputEvent event;
	event.type = InputEventType::KEY_UP;
	event.keyCode = VK_KEY;
	Notify(event);
}

void InputSystem::OnMouseMove(int curX, int curY)
{
	InputEvent event;
	event.type = InputEventType::MOUSE_MOVE;
	event.mouseDeltaX = curX - (m_ChkPosFirst ? curX : m_OldMousePos.x);
	event.mouseDeltaY = curY - (m_ChkPosFirst ? curY : m_OldMousePos.y);
	if (m_ChkPosFirst) m_ChkPosFirst = false;
	m_OldMousePos.x = event.mouseX = curX;
	m_OldMousePos.y = event.mouseY = curY;
	Notify(event);
}

void InputSystem::OnMouseMoveCenter(HWND hWnd, int curX, int curY)
{
	InputEvent event;
	event.type = InputEventType::MOUSE_MOVE;

	// 1. 게임 창의 클라이언트 영역 크기를 가져옵니다. (넘겨받은 hWnd 사용)
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// 2. 클라이언트 영역의 중앙 좌표를 계산합니다. (클라이언트 좌표 기준)
	int clientCenterX = (clientRect.right - clientRect.left) / 2;
	int clientCenterY = (clientRect.bottom - clientRect.top) / 2;

	// 3. 델타 계산
	event.mouseDeltaX = curX - clientCenterX;
	event.mouseDeltaY = curY - clientCenterY;

	// 4. 클라이언트 중앙 좌표를 화면 좌표로 변환합니다. (넘겨받은 hWnd 사용)
	POINT screenCenterPos = { clientCenterX, clientCenterY };
	ClientToScreen(hWnd, &screenCenterPos);						//화면좌상단(0,0) 이 아니므로 상대좌표값을계산한다

	// 5. 마우스 커서를 화면 중앙 (변환된 좌표)으로 재배치합니다.
	SetCursorPos(screenCenterPos.x, screenCenterPos.y);

	// 6. 이벤트에 현재 마우스 위치 (화면 좌표) 저장 (선택 사항)
	event.mouseX = screenCenterPos.x;
	event.mouseY = screenCenterPos.y;

	Notify(event);
}

void InputSystem::Notify(const InputEvent& event)
{
	if (m_Listners[event.type].size() <= 0) return;
	for (const auto& iter : m_Listners[event.type])
		if (iter.second)
			iter.second(event);
}

void InputSystem::Init()
{
	std::memset(m_CurKeystate, 0, sizeof(m_CurKeystate));
	std::memset(m_OldKeystate, 0, sizeof(m_CurKeystate));
}

void InputSystem::Frame()
{
	for (int i = 0; i < 256; i++)
	{
		if (m_OldKeystate[i] && m_CurKeystate[i])
			OnKeyPressed(i);
	}
	std::memcpy(m_OldKeystate, m_CurKeystate, sizeof(m_OldKeystate));
}

void InputSystem::Release()
{
	m_Listners.clear();
}
