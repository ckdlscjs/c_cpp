#include "InputSystem.h"


InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
	m_Listners.clear();
}


void InputSystem::Init()
{
	std::memset(m_bOldKeyStates, false, sizeof(m_bOldKeyStates));
	std::memset(m_bCurKeyStates, false, sizeof(m_bCurKeyStates));
}

void InputSystem::Frame()
{
	std::cout << "Frame : " << "InputSystem" << " Class" << '\n';
	for (int i = 0; i < 256; i++)
	{
		if (GetKeyState(i) == E_InputEvent::KEY_PRESSED)
			OnKeyPressed(i);
	}
	std::memcpy(m_bOldKeyStates, m_bCurKeyStates, sizeof(m_bOldKeyStates));	//��������(���������ӿ��ݿ�)
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

void InputSystem::Notify(const InputEvent& event)
{
	if (m_Listners[event.type].size() <= 0) return;
	for (const auto& iter : m_Listners[event.type])
		if (iter.second)
			iter.second(event);
}

void InputSystem::OnKeyDown(unsigned char VK_KEY)
{
	_ASEERTION_NULCHK(0 <= VK_KEY && VK_KEY <= 255, "VK_KEY Invalid");
	if (m_bOldKeyStates[VK_KEY]) return;
	std::cout << "OnKeyDown " << VK_KEY << '\n';
	m_bCurKeyStates[VK_KEY] = true;
	InputEvent event;
	event.type = E_InputEvent::KEY_DOWN;
	event.keyCode = VK_KEY;
	Notify(event);
}

void InputSystem::OnKeyPressed(unsigned char VK_KEY)
{
	_ASEERTION_NULCHK(0 <= VK_KEY && VK_KEY <= 255, "VK_KEY Invalid");
	std::cout << "OnKeyPressed " << VK_KEY << '\n';
	InputEvent event;
	event.type = E_InputEvent::KEY_PRESSED;
	event.keyCode = VK_KEY;
	Notify(event);
}

void InputSystem::OnKeyUp(unsigned char VK_KEY)
{
	_ASEERTION_NULCHK(0 <= VK_KEY && VK_KEY <= 255, "VK_KEY Invalid");
	if (!m_bOldKeyStates[VK_KEY]) return;
	std::cout << "OnKeyUp " << VK_KEY << '\n';
	m_bCurKeyStates[VK_KEY] = false;
	InputEvent event;
	event.type = E_InputEvent::KEY_UP;
	event.keyCode = VK_KEY;
	Notify(event);
}

E_InputEvent InputSystem::GetKeyState(unsigned char VK_KEY) const
{
	_ASEERTION_NULCHK(0 <= VK_KEY && VK_KEY <= 255, "VK_KEY Invalid");
	if (m_bOldKeyStates[VK_KEY] && m_bCurKeyStates[VK_KEY]) return E_InputEvent::KEY_PRESSED;
	if (!m_bOldKeyStates[VK_KEY] && m_bCurKeyStates[VK_KEY]) return E_InputEvent::KEY_DOWN;
	if (m_bOldKeyStates[VK_KEY] && !m_bCurKeyStates[VK_KEY]) return E_InputEvent::KEY_UP;
	return E_InputEvent::NOTHING;
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

