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
	void AddAnimbones(size_t hash);
	const std::vector<Matrix4x4>& GetAnimbones(size_t hash);

private:
	float fPlayRate = 0.1f;
	std::unordered_map<size_t, std::vector<Matrix4x4>> m_matAnimbones;
};
#define _AnimationSystem AnimationSystem::GetInstance()