#include "InputSystem.h"
#include "Components.h"
#include "ECSSystem.h"
InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
	m_Listners.clear();
}


void InputSystem::Init()
{
}

void InputSystem::Frame()
{
	std::cout << "Frame : " << "InputSystem" << " Class" << '\n';
	for (int i = 0; i < 256; i++)
	{
		if (GetKeyState(i) == E_InputEvent::KEY_PRESSED)
			OnKeyPressed(i);
	}
	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Input>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
	//Input이 있는 인덱스를 기록하고 뒤로 밀집시킨다
	for (auto& archetype : queries)
	{
		std::vector<std::pair<size_t, size_t>> chks;
		for (size_t row = 0; row < archetype->GetCount_Chunks(); row++)
		{
			auto& chunks = archetype->GetComponents<C_Input>(row);
			for (size_t col = 0; col < chunks.size(); col++)
			{
				if ((chunks[col].vk_mask & m_bCurKeyStates) != 0) 
					chks.push_back({ row, col });
			}
		}
		size_t startIdx = archetype->GetCount() - chks.size();
		size_t curIdx = startIdx;
		//청크의 뒤에 배치시킨다(Swap)
		for (const auto& iter : chks)
		{
			size_t srcRow = iter.first;
			size_t srcCol = iter.second;
			size_t destRow = curIdx / archetype->GetCapcity_Chunk();
			size_t destCol = curIdx % archetype->GetCapcity_Chunk();
			auto src_dest = archetype->SwapChunkData(srcRow, srcCol, destRow, destCol);
			size_t lookupSrc = src_dest.first;
			size_t lookupDest = src_dest.second;
			_ECSSystem.Swap(lookupSrc, lookupDest);
			curIdx++;
		}
		archetype->m_transfer_row = startIdx / archetype->GetCapcity_Chunk();
		archetype->m_transfer_col = startIdx % archetype->GetCapcity_Chunk();
	}
	m_bOldKeyStates = m_bCurKeyStates; //상태전이(다음프레임에반영)
}
size_t InputSystem::AddListner(E_InputEvent type, EventCallBack callback)
{
	if (!callback) return 0; //유효한 콜백이 아니면 (0, 오류)
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

void InputSystem::SetMouseCenter(HWND hWnd)
{
	// 1. 게임 창의 클라이언트 영역 크기를 가져옵니다. (넘겨받은 hWnd 사용)
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// 2. 클라이언트 영역의 중앙 좌표를 계산합니다. (클라이언트 좌표 기준)
	int clientCenterX = (clientRect.right - clientRect.left) / 2;
	int clientCenterY = (clientRect.bottom - clientRect.top) / 2;

	// 4. 클라이언트 중앙 좌표를 화면 좌표로 변환합니다. (넘겨받은 hWnd 사용)
	POINT screenCenterPos = { clientCenterX, clientCenterY };
	ClientToScreen(hWnd, &screenCenterPos);						//화면좌상단(0,0) 이 아니므로 상대좌표값을계산한다

	// 5. 마우스 커서를 화면 중앙 (변환된 좌표)으로 재배치합니다.
	SetCursorPos(screenCenterPos.x, screenCenterPos.y);
}

