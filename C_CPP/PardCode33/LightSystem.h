#pragma once
#include "BaseSystem.h"

//not need LightSys?
class BaseLight;

class LightSystem : public BaseSystem<LightSystem>
{
	friend class BaseSystem<LightSystem>;	//CRTP패턴을 위한 friend선언
private:
	LightSystem();
	LightSystem(const LightSystem&) = delete;
	LightSystem& operator=(const LightSystem&) = delete;
	LightSystem(LightSystem&&) = delete;
	LightSystem& operator=(LightSystem&&) = delete;

public:
	void Init();
	void Frame();
	~LightSystem();

	//임시
	size_t lookup_directional;
	size_t lookup_point;
	size_t lookup_spot;
private:
	
};
#define _LightSystem LightSystem::GetInstance()
