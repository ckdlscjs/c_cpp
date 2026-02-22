#pragma once
#include "BaseSystem.h"

class InputSystem : public BaseSystem<InputSystem>
{
	friend class BaseSystem<InputSystem>;	//CRTP패턴을 위한 friend선언
private:
	InputSystem();
	InputSystem(const InputSystem&) = delete;
	InputSystem& operator=(const InputSystem&) = delete;
	InputSystem(InputSystem&&) = delete;
	InputSystem& operator=(InputSystem&&) = delete;
public:
	~InputSystem();
	void Init();
	void Frame();
	size_t AddListner(E_InputEvent type, EventCallBack callback);
	void RemoveListner(E_InputEvent type, size_t id);
	void Notify(const InputEvent& event);
	void OnKeyDown(unsigned char VK_KEY);
	void OnKeyPressed(unsigned char VK_KEY);
	void OnKeyUp(unsigned char VK_KEY);
	const E_InputEvent* GetKeysState();
	void SetMousePos(int curX, int curY);
	void OnMouseMove(int curX, int curY);
	void OnMouseMoveCenter(HWND hWnd, int curX, int curY);
	void SetMouseCenter(HWND hWnd);
	Vector2 GetMouseDelta() const;
	bool IsPressed_LBTN() const;
	bool IsPressed_RBTN() const;
private:
	std::unordered_map<E_InputEvent, std::unordered_map<size_t, EventCallBack>> m_Listners;
	size_t m_CallbackID = 1;	//0은 오류체크
	Vector2 m_OldMousePos;
	Vector2 m_CurMousePos;
	Vector2 m_MouseDelta;
	std::bitset<256> m_bOldKeyStates;
	std::bitset<256> m_bCurKeyStates;
	E_InputEvent m_eKeyStates[256];
	bool m_bCenterMouse = true;
	bool m_bChkPosFirst = true;
};
#define _InputSystem InputSystem::GetInstance()

