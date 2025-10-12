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
	size_t AddListner(E_InputEvent type, EventCallBack callback);
	void RemoveListner(E_InputEvent type, size_t id);
	void Notify(const InputEvent& event);
	void OnKeyDown(unsigned char VK_KEY);
	void OnKeyPressed(unsigned char VK_KEY);
	void OnKeyUp(unsigned char VK_KEY);
	bool GetKeyState(unsigned char VK_KEY) const;
	void OnMouseMove(int curX, int curY);
	void OnMouseMoveCenter(HWND hWnd, int curX, int curY);
	void SetMouseCenter(HWND hWnd);
	
private:
	std::unordered_map<E_InputEvent, std::unordered_map<size_t, EventCallBack>> m_Listners;
	size_t m_CallbackID = 1;	//0은 오류체크
	PointXY m_OldMousePos;
	bool m_bKeyStates[256];
	bool m_bCenterMouse = true;
	bool m_bChkPosFirst = true;
};
#define _InputSystem InputSystem::GetInstance()

