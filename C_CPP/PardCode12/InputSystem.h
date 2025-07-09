#pragma once
#include "std.h"

class InputSystem
{
private:
	InputSystem();
	~InputSystem();
	InputSystem(InputSystem& inputSystem) = delete;
	InputSystem& operator=(const InputSystem& inputSystem) = delete;
public:
	static InputSystem& GetInstance();	//Singleton함수, 런타임중 호출시 정적변수로써 생성된다
	void Frame();
	size_t AddListner(InputEventType type, EventCallBack callback);
	void RemoveListner(InputEventType type, size_t id);
private:
	std::unordered_map<InputEventType, std::unordered_map<size_t, EventCallBack>> m_Listners;
	size_t m_CallbackID = 1;	//0은 오류체크
	unsigned char m_CurKeystate[256];
	unsigned char m_OldKeystate[256];
};
#define _InputSystem InputSystem::GetInstance()

