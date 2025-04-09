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
	// Ű �迭�� �ʱ�ȭ�մϴ�
	for (int i = 0; i < 256; ++i)
	{
		m_keys[i] = false;
	}
}

// Ű�� ���ȴٸ� �ش� Ű���� true�� �����մϴ�
void InputClass::KeyDown(unsigned int input)
{
	m_keys[input] = true;
}

// Ű�� �����Ǿ��ٸ� �ش� Ű���� false�� �����մϴ�
void InputClass::KeyUp(unsigned int input)
{
	m_keys[input] = false;
}

// ���� Ű���� ���������� �ƴ��� ���¸� ��ȯ�մϴ�
bool InputClass::IsKeyDown(unsigned int input)
{
	return m_keys[input];
}
