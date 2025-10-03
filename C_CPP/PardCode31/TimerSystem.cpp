#include "TimerSystem.h"
TimerSystem::TimerSystem()
{
}

void TimerSystem::Init()
{
	//FPS를 체크하기위한 프로그램의 시작타임
	m_iStartTime = GetTickCount64();
	m_iCount = 0;
	m_iFps = 0;
	
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency);	//1000밀리초당 카운터의 수치값
	QueryPerformanceCounter((LARGE_INTEGER*)&m_StartCount);		//시작카운트
}

void TimerSystem::Frame()
{
	//fps계산, 1초단위(1000ms)
	m_iCount++;
	if (GetTickCount64() >= m_iStartTime + 1000)
	{
		m_iFps = m_iCount;
		m_iCount = 0;
		m_iStartTime = GetTickCount64();
	}

	//deltatime계산, 1프레임단위
	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrentCount);
	ULONGLONG diff = m_CurrentCount - m_StartCount;
	m_fDeltaTime = (float)diff / (float)m_Frequency;
	m_StartCount = m_CurrentCount;
	m_fElpasedTime += m_fDeltaTime;
}

void TimerSystem::Release()
{
}

UINT TimerSystem::GetFps() const
{
	return m_iFps;
}

float TimerSystem::GetDeltaTime() const
{
	return m_fDeltaTime;
}

float TimerSystem::GetElapsedTime() const
{
	return m_fElpasedTime;
}


