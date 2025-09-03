#pragma once
#include "CommonHeader.h"
/*
* Collider�� ���� ������������ ������ ��ȯ����� �޾Ƽ� ��ȯ�ϴ� ���·� �����Ѵ�
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