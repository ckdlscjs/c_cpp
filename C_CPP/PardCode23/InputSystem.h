#pragma once
#include "BaseSystem.h"

class InputSystem : public BaseSystem<InputSystem>
{
	friend class BaseSystem<InputSystem>;	//CRTP������ ���� friend����
private:
	InputSystem();
	~InputSystem();
	InputSystem(const InputSystem&) = delete;
	InputSystem& operator=(const InputSystem&) = delete;
	InputSystem(InputSystem&&) = delete;
	InputSystem& operator=(InputSystem&&) = delete;
public:
	void Init();
	void Frame();
	void Release();

	size_t AddListner(InputEventType type, EventCallBack callback);
	void RemoveListner(InputEventType type, size_t id);
	void Notify(const InputEvent& event);

	void OnKeyDown(unsigned char VK_KEY);
	void OnKeyPressed(unsigned char VK_KEY);
	void OnKeyUp(unsigned char VK_KEY);
	bool GetKeyState(unsigned char VK_KEY) const;
	void OnMouseMove(int curX, int curY);
	void OnMouseMoveCenter(HWND hWnd, int curX, int curY);
	void SetMouseCenter(HWND hWnd);
	
private:
	std::unordered_map<InputEventType, std::unordered_map<size_t, EventCallBack>> m_Listners;
	size_t m_CallbackID = 1;	//0�� ����üũ
	bool m_CurKeystate[256];
	bool m_OldKeystate[256];
	
	PointXY m_OldMousePos;
	bool m_ChkPosFirst = true;
};
#define _InputSystem InputSystem::GetInstance()

