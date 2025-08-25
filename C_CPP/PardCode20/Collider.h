#pragma once
#include "CommonHeader.h"
/*
* Collider는 로컬 기하정보만을 가지며 변환행렬을 받아서 변환하는 형태로 수행한다
*/
class Collider
{
public:
	inline Collider() {}
	inline virtual ~Collider() {}
	inline void SetType(Colliders type) { m_type = type; }
	inline Colliders GetType() const { return m_type; }
private:
	Colliders m_type = Colliders::NONE;
};