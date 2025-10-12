#pragma once
#include "CommonHeader.h"
/*
* Collider�� ���� ������������ ������ ��ȯ����� �޾Ƽ� ��ȯ�ϴ� ���·� �����Ѵ�
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