#pragma once
#include "BaseSystem.h"

class UnorderedAccessView;

class ComputeSystem : public BaseSystem<ComputeSystem>
{
	friend class BaseSystem<ComputeSystem>;	//CRTP패턴을 위한 friend선언
private:
	ComputeSystem();
	ComputeSystem(const ComputeSystem&) = delete;
	ComputeSystem& operator=(const ComputeSystem&) = delete;
	ComputeSystem(ComputeSystem&&) = delete;
	ComputeSystem& operator=(ComputeSystem&&) = delete;

public:
	~ComputeSystem();
	void Init();
	void Update();


private:
};
#define _ComputeSystem ComputeSystem::GetInstance()