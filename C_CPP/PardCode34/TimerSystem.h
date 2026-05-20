#pragma once
#include "BaseSystem.h"

class TimerSystem : public BaseSystem<TimerSystem>
{
	friend class BaseSystem<TimerSystem>;	//CRTP패턴을 위한 friend선언
private:
	TimerSystem();
	TimerSystem(const TimerSystem&) = delete;
	TimerSystem& operator=(const TimerSystem&) = delete;
	TimerSystem(TimerSystem&&) = delete;
	TimerSystem& operator=(TimerSystem&&) = delete;
public:
	~TimerSystem();
	void Init();
	void Frame();
	UINT GetFps() const;
	float GetDeltaTime() const;
	float GetElapsedTime() const;
	void ScopedBegin();
	double ScopedEnd();
private:
	UINT m_iCount;
	UINT m_iFps;
	ULONGLONG m_iStartTime;
	ULONGLONG m_Frequency;
	ULONGLONG m_StartCount;
	ULONGLONG m_CurrentCount;
	float m_fDeltaTime;
	float m_fElpasedTime;
	std::chrono::steady_clock::time_point m_ScopedStart;
};
#define _TimerSystem TimerSystem::GetInstance()