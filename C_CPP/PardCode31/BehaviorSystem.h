#pragma once
#include "BaseSystem.h"
class BehaviorSystem : public BaseSystem<BehaviorSystem>
{
	friend class BaseSystem<BehaviorSystem>;	//CRTP패턴을 위한 friend선언
private:
	BehaviorSystem();
	BehaviorSystem(const BehaviorSystem&) = delete;
	BehaviorSystem& operator=(const BehaviorSystem&) = delete;
	BehaviorSystem(BehaviorSystem&&) = delete;
	BehaviorSystem& operator=(BehaviorSystem&&) = delete;
public:
	~BehaviorSystem();
	void Init();
	void Frame(float deltatime);

private:

};
#define _BehaviorSystem BehaviorSystem::GetInstance()