#include "InputSystem.h"


InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
	m_Listners.clear();
}
size_t InputSystem::AddListner(E_InputEvent type, EventCallBack callback)
{
	if (!callback) return 0; //��ȿ�� �ݹ��� �ƴϸ� (0, ����)
	m_Listners[type][m_CallbackID] = callback;
	return m_CallbackID++;
}

void InputSystem::RemoveListner(E_InputEvent type, size_t id)
{
	if (m_Listners[type].empty()) return;
	m_Listners[type].erase(id);
}

void InputSystem::OnKeyDown(unsigned char VK_KEY)
{
	if (VK_KEY < 0 || VK_KEY > 255) return;
	if (m_bKeyStates[VK_KEY]) return;
	m_bKeyStates[VK_KEY] = true;
	InputEvent event;
	event.type = E_InputEvent::KEY_DOWN;
	event.keyCode = VK_KEY;
	Notify(event);
}

void InputSystem::OnKeyPressed(unsigned char VK_KEY)
{
	if (VK_KEY < 0 || VK_KEY > 255) return;
	InputEvent event;
	event.type = E_InputEvent::KEY_PRESSED;
	event.keyCode = VK_KEY;
	Notify(event);
}

void InputSystem::OnKeyUp(unsigned char VK_KEY)
{
	if (VK_KEY < 0 || VK_KEY > 255) return;
	if (!m_bKeyStates[VK_KEY]) return;
	m_bKeyStates[VK_KEY] = false;
	InputEvent event;
	event.type = E_InputEvent::KEY_UP;
	event.keyCode = VK_KEY;
	Notify(event);
}

bool InputSystem::GetKeyState(unsigned char VK_KEY) const
{
	if (VK_KEY < 0 || VK_KEY > 255) return false;
	return m_bKeyStates[VK_KEY];
}

void InputSystem::OnMouseMove(int curX, int curY)
{
	InputEvent event;
	event.type = E_InputEvent::MOUSE_MOVE;
	event.mouseDeltaX = curX - (m_bChkPosFirst ? curX : m_OldMousePos.x);
	event.mouseDeltaY = curY - (m_bChkPosFirst ? curY : m_OldMousePos.y);
	if (m_bChkPosFirst) m_bChkPosFirst = false;
	m_OldMousePos.x = event.mouseX = curX;
	m_OldMousePos.y = event.mouseY = curY;
	Notify(event);
}

void InputSystem::OnMouseMoveCenter(HWND hWnd, int curX, int curY)
{
	InputEvent event;
	event.type = E_InputEvent::MOUSE_MOVE;

	// 1. ���� â�� Ŭ���̾�Ʈ ���� ũ�⸦ �����ɴϴ�. (�Ѱܹ��� hWnd ���)
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// 2. Ŭ���̾�Ʈ ������ �߾� ��ǥ�� ����մϴ�. (Ŭ���̾�Ʈ ��ǥ ����)
	int clientCenterX = (clientRect.right - clientRect.left) / 2;
	int clientCenterY = (clientRect.bottom - clientRect.top) / 2;

	// 3. ��Ÿ ���
	event.mouseDeltaX = curX - clientCenterX;
	event.mouseDeltaY = curY - clientCenterY;

	// 4. Ŭ���̾�Ʈ �߾� ��ǥ�� ȭ�� ��ǥ�� ��ȯ�մϴ�. (�Ѱܹ��� hWnd ���)
	POINT screenCenterPos = { clientCenterX, clientCenterY };
	ClientToScreen(hWnd, &screenCenterPos);						//ȭ���»��(0,0) �� �ƴϹǷ� �����ǥ��������Ѵ�

	// 5. ���콺 Ŀ���� ȭ�� �߾� (��ȯ�� ��ǥ)���� ���ġ�մϴ�.
	SetCursorPos(screenCenterPos.x, screenCenterPos.y);

	// 6. �̺�Ʈ�� ���� ���콺 ��ġ (ȭ�� ��ǥ) ���� (���� ����)
	event.mouseX = screenCenterPos.x;
	event.mouseY = screenCenterPos.y;

	Notify(event);
}

void InputSystem::SetMouseCenter(HWND hWnd)
{
	// 1. ���� â�� Ŭ���̾�Ʈ ���� ũ�⸦ �����ɴϴ�. (�Ѱܹ��� hWnd ���)
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// 2. Ŭ���̾�Ʈ ������ �߾� ��ǥ�� ����մϴ�. (Ŭ���̾�Ʈ ��ǥ ����)
	int clientCenterX = (clientRect.right - clientRect.left) / 2;
	int clientCenterY = (clientRect.bottom - clientRect.top) / 2;

	// 4. Ŭ���̾�Ʈ �߾� ��ǥ�� ȭ�� ��ǥ�� ��ȯ�մϴ�. (�Ѱܹ��� hWnd ���)
	POINT screenCenterPos = { clientCenterX, clientCenterY };
	ClientToScreen(hWnd, &screenCenterPos);						//ȭ���»��(0,0) �� �ƴϹǷ� �����ǥ��������Ѵ�

	// 5. ���콺 Ŀ���� ȭ�� �߾� (��ȯ�� ��ǥ)���� ���ġ�մϴ�.
	SetCursorPos(screenCenterPos.x, screenCenterPos.y);
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
	std::memset(m_bKeyStates, false, sizeof(m_bKeyStates));
}


