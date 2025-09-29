#include "TimerSystem.h"
TimerSystem::TimerSystem()
{
}

void TimerSystem::Init()
{
	//FPS�� üũ�ϱ����� ���α׷��� ����Ÿ��
	m_iStartTime = GetTickCount64();
	m_iCount = 0;
	m_iFps = 0;
	
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency);	//1000�и��ʴ� ī������ ��ġ��
	QueryPerformanceCounter((LARGE_INTEGER*)&m_StartCount);		//����ī��Ʈ
}

void TimerSystem::Frame()
{
	//fps���, 1�ʴ���(1000ms)
	m_iCount++;
	if (GetTickCount64() >= m_iStartTime + 1000)
	{
		m_iFps = m_iCount;
		m_iCount = 0;
		m_iStartTime = GetTickCount64();
	}

	//deltatime���, 1�����Ӵ���
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


