#pragma once
#include "BaseSystem.h"

class MovementSystem : public BaseSystem<MovementSystem>
{
	friend class BaseSystem<MovementSystem>;	//CRTP패턴을 위한 friend선언
private:
	MovementSystem();
	MovementSystem(const MovementSystem&) = delete;
	MovementSystem& operator=(const MovementSystem&) = delete;
	MovementSystem(MovementSystem&&) = delete;
	MovementSystem& operator=(MovementSystem&&) = delete;
public:
	~MovementSystem();
	void Init();
	void Frame(float deltatime);

private:

};
#define _MovementSystem MovementSystem::GetInstance()