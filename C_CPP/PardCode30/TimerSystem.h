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
	void Init();
	void Frame();
	void Release();
	UINT GetFps() const;
	float GetDeltaTime() const;
	float GetElapsedTime() const;
private:
	UINT m_iCount;
	UINT m_iFps;
	ULONGLONG m_iStartTime;
	ULONGLONG m_Frequency;
	ULONGLONG m_StartCount;
	ULONGLONG m_CurrentCount;
	float m_fDeltaTime;
	float m_fElpasedTime;
};
#define _TimerSystem TimerSystem::GetInstance()