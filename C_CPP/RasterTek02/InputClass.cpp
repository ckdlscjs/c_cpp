#include "stdafx.h"
#include "InputClass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass&)
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	// 키 배열을 초기화합니다
	for (int i = 0; i < 256; ++i)
	{
		m_keys[i] = false;
	}
}

// 키가 눌렸다면 해당 키값을 true로 저장합니다
void InputClass::KeyDown(unsigned int input)
{
	m_keys[input] = true;
}

// 키가 해제되었다면 해당 키값을 false로 저장합니다
void InputClass::KeyUp(unsigned int input)
{
	m_keys[input] = false;
}

// 현재 키값이 눌려졌는지 아닌지 상태를 반환합니다
bool InputClass::IsKeyDown(unsigned int input)
{
	return m_keys[input];
}
