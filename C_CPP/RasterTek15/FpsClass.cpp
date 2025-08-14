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

	//timeGetTime은 시작이후 경과시간을 밀리초단위로 반환한다, +1000은 1초의 경과를 의미하므로 1초마다의 프레임계산을 의미한다
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