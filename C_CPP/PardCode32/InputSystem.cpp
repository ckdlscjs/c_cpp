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

	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Input>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);
	//Input이 있는 인덱스를 기록하고 뒤로 밀집시킨다
	for (auto& archetype : queries)
	{
		archetype->m_transfer_row = archetype->m_transfer_col = 0;	//inputSys는 스타트시점이므로 transfer초기화
		size_t st_row = archetype->m_transfer_row;
		size_t st_col = archetype->m_transfer_col;
		std::vector<std::pair<size_t, size_t>> swaps;
		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
		{
			auto& chunks = archetype->GetComponents<C_Input>(row);
			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
			{
				if ((chunks[col].bVKMask & m_bCurKeyStates) != 0)
				{
					swaps.push_back({ row, col });
				}
			}
		}
		size_t startIdx = archetype->GetAllChunkCount() - swaps.size();
		//어떠한 Chunk가 Component우선순위에 따라 정렬을 할경우 중복된 컴포넌트(input, move, 등 동시우선순위에 의해 앞으로 돌리려면 예외처리가 힘들다)
		_ECSSystem.UpdateSwapChunk(swaps, startIdx, archetype);
	}
	
	//Mouse Delta
	m_MouseDelta = m_CurMousePos - m_OldMousePos;

	for (unsigned char vk_key = 0; vk_key < 255; vk_key++)	//255번은 마우스체크(임시)
	{
		if (m_bOldKeyStates[vk_key] && m_bCurKeyStates[vk_key]) m_eKeyStates[vk_key] = E_InputEvent::KEY_PRESSED;
		else if (!m_bOldKeyStates[vk_key] && m_bCurKeyStates[vk_key]) m_eKeyStates[vk_key] = E_InputEvent::KEY_DOWN;
		else if (m_bOldKeyStates[vk_key] && !m_bCurKeyStates[vk_key]) m_eKeyStates[vk_key] = E_InputEvent::KEY_UP;
		else m_eKeyStates[vk_key] = E_InputEvent::NOTHING;
	}

	//상태전이(다음프레임에반영)
	m_OldMousePos = m_CurMousePos;
	m_bOldKeyStates = m_bCurKeyStates; 
	m_bCurKeyStates[255] = false;
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
	//std::cout << "OnKeyDown " << VK_KEY << '\n';
	m_bCurKeyStates[VK_KEY] = true;
	InputEvent event;
	event.type = E_InputEvent::KEY_DOWN;
	event.keyCode = VK_KEY;
	Notify(event);
}

void InputSystem::OnKeyPressed(unsigned char VK_KEY)
{
	_ASEERTION_NULCHK(0 <= VK_KEY && VK_KEY <= 255, "VK_KEY Invalid");
	//std::cout << "OnKeyPressed " << VK_KEY << '\n';
	InputEvent event;
	event.type = E_InputEvent::KEY_PRESSED;
	event.keyCode = VK_KEY;
	Notify(event);
}

void InputSystem::OnKeyUp(unsigned char VK_KEY)
{
	_ASEERTION_NULCHK(0 <= VK_KEY && VK_KEY <= 255, "VK_KEY Invalid");
	if (!m_bOldKeyStates[VK_KEY]) return;
	//std::cout << "OnKeyUp " << VK_KEY << '\n';
	m_bCurKeyStates[VK_KEY] = false;
	InputEvent event;
	event.type = E_InputEvent::KEY_UP;
	event.keyCode = VK_KEY;
	
	Notify(event);
}

const E_InputEvent* InputSystem::GetKeysState()
{
	return m_eKeyStates;
}

void InputSystem::SetMousePos(int curX, int curY)
{
	int a = 0;
}

void InputSystem::OnMouseMove(int curX, int curY)
{
	if (m_bChkPosFirst) m_bChkPosFirst = false;
	m_bCurKeyStates[255] = true;
	m_CurMousePos.Set(curX, curY);

	InputEvent event;
	/*
	* 추후수정필요
	event.type = E_InputEvent::MOUSE_MOVE;
	event.mouseDeltaX = curX - (m_bChkPosFirst ? curX : m_OldMousePos.GetX());
	event.mouseDeltaY = curY - (m_bChkPosFirst ? curY : m_OldMousePos.GetY());
	event.mouseX = curX;
	event.mouseY = curY;
	*/
	Notify(event);
}

void InputSystem::OnMouseMoveCenter(HWND hWnd, int curX, int curY)
{
	// 1. 게임 창의 클라이언트 영역 크기를 가져옵니다. (넘겨받은 hWnd 사용)
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// 2. 클라이언트 영역의 중앙 좌표를 계산합니다. (클라이언트 좌표 기준)
	int clientCenterX = (clientRect.right - clientRect.left) / 2;
	int clientCenterY = (clientRect.bottom - clientRect.top) / 2;

	// 3. 델타 계산
	m_bCurKeyStates[255] = true;
	m_OldMousePos = { (float)clientCenterX, (float)clientCenterY };
	m_CurMousePos.Set(curX, curY);

	// 4. 클라이언트 중앙 좌표를 화면 좌표로 변환합니다. (넘겨받은 hWnd 사용)
	POINT screenCenterPos = { clientCenterX, clientCenterY };
	ClientToScreen(hWnd, &screenCenterPos);						//화면좌상단(0,0) 이 아니므로 상대좌표값을계산한다

	// 5. 마우스 커서를 화면 중앙 (변환된 좌표)으로 재배치합니다.
	SetCursorPos(screenCenterPos.x, screenCenterPos.y);

	InputEvent event;
	event.type = E_InputEvent::MOUSE_MOVE;

	event.mouseDeltaX = curX - clientCenterX;
	event.mouseDeltaY = curY - clientCenterY;

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

Vector2 InputSystem::GetMouseDelta() const
{
	return m_MouseDelta;
}

bool InputSystem::IsPressed_LBTN() const
{
	return m_bCurKeyStates[VK_LBUTTON];
}

bool InputSystem::IsPressed_RBTN() const
{
	return m_bCurKeyStates[VK_RBUTTON];
}

