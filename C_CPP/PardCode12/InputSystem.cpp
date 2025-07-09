#include "InputSystem.h"


InputSystem::InputSystem()
{
	std::cout << "InputSystem" << " Class" << " 생성자 호출" << '\n';
	std::memset(m_CurKeystate, 0, sizeof(m_CurKeystate));
	std::memset(m_OldKeystate, 0, sizeof(m_CurKeystate));
}

InputSystem::~InputSystem()
{
	std::cout << "InputSystem" << " Class" << " 소멸자 호출" << '\n';
	m_Listners.clear();
}

//싱글톤객체, static선언으로 컴파일타임에 객체가 생성된다
InputSystem& InputSystem::GetInstance()
{
	static InputSystem engine;
	return engine;
}

void InputSystem::Frame()
{

}

size_t InputSystem::AddListner(InputEventType type, EventCallBack callback)
{
	if (!callback) return 0;
	m_Listners[type][m_CallbackID] = callback;
	return m_CallbackID++;
}

void InputSystem::RemoveListner(InputEventType type, size_t id)
{
	if (m_Listners[type].empty()) return;
	m_Listners[type].erase(id);
}
