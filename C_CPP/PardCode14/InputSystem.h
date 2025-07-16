#pragma once
#include "BaseSystem.h"

class InputSystem : public BaseSystem<InputSystem>
{
	friend class BaseSystem<InputSystem>;	//CRTP패턴을 위한 friend선언
private:
	InputSystem();
	~InputSystem();
	InputSystem(const InputSystem&) = delete;
	InputSystem& operator=(const InputSystem&) = delete;
	InputSystem(InputSystem&&) = delete;
	InputSystem& operator=(InputSystem&&) = delete;
public:
	size_t AddListner(InputEventType type, EventCallBack callback);
	void RemoveListner(InputEventType type, size_t id);
	void OnKeyDown(unsigned char VK_KEY);
	void OnKeyPressed(unsigned char VK_KEY);
	void OnKeyUp(unsigned char VK_KEY);
	void OnMouseMove(int curX, int curY);

	void Notify(const InputEvent& event);
	void Frame();
	void Release();
private:
	std::unordered_map<InputEventType, std::unordered_map<size_t, EventCallBack>> m_Listners;
	size_t m_CallbackID = 1;	//0은 오류체크
	bool m_CurKeystate[256];
	bool m_OldKeystate[256];
	
	PointXY m_OldMousePos;
	bool m_ChkPosFirst = true;
};
#define _InputSystem InputSystem::GetInstance()

