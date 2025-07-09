#include "InputSystem.h"


InputSystem::InputSystem()
{
	std::cout << "InputSystem" << " Class" << " ������ ȣ��" << '\n';
	std::memset(m_CurKeystate, 0, sizeof(m_CurKeystate));
	std::memset(m_OldKeystate, 0, sizeof(m_CurKeystate));
}

InputSystem::~InputSystem()
{
	std::cout << "InputSystem" << " Class" << " �Ҹ��� ȣ��" << '\n';
	m_Listners.clear();
}

//�̱��水ü, static�������� ������Ÿ�ӿ� ��ü�� �����ȴ�
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
