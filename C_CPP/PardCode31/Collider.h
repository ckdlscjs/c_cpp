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
	inline void SetType(E_Colliders type) { m_type = type; }
	inline E_Colliders GetType() const { return m_type; }
private:
	E_Colliders m_type = E_Colliders::NONE;
};