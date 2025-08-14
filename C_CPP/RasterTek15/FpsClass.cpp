#include "stdafx.h"
#include <mmsystem.h>
#include "FpsClass.h"


FpsClass::FpsClass()
{
}


FpsClass::FpsClass(const FpsClass& other)
{
}


FpsClass::~FpsClass()
{
}


void FpsClass::Initialize()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
	return;
}


void FpsClass::Frame()
{
	m_count++;

	//timeGetTime�� �������� ����ð��� �и��ʴ����� ��ȯ�Ѵ�, +1000�� 1���� ����� �ǹ��ϹǷ� 1�ʸ����� �����Ӱ���� �ǹ��Ѵ�
	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
}


int FpsClass::GetFps()
{
	return m_fps;
}