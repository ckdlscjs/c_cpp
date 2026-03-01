#pragma once
#include "BaseSystem.h"
class AnimationSystem : public BaseSystem<AnimationSystem>
{
	friend class BaseSystem<AnimationSystem>;	//CRTP패턴을 위한 friend선언
private:
	AnimationSystem();
	AnimationSystem(const AnimationSystem&) = delete;
	AnimationSystem& operator=(const AnimationSystem&) = delete;
	AnimationSystem(AnimationSystem&&) = delete;
	AnimationSystem& operator=(AnimationSystem&&) = delete;

public:
	~AnimationSystem();
	void Init();
	void Frame(float deltatime);

};
#define _AnimationSystem AnimationSystem::GetInstance()