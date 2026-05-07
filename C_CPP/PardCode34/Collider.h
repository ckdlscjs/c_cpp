#pragma once
#include "CommonHeader.h"
/*
* Collider는 로컬 기하정보만을 가지며 변환행렬을 받아서 변환하는 형태로 수행한다
*/
class Collider
{
protected:
	inline Collider() {}
public:
	inline virtual ~Collider() {}
	inline void SetType(E_Collider type) { m_type = type; }
	inline E_Collider GetType() const { return m_type; }
private:
	E_Collider m_type = E_Collider::NONE;
};