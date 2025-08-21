#pragma once
#include "Plane.h"

__declspec(align(16))
class Frustum
{
public:
	inline Frustum() = default;
	//v * p ������� ����ü�� �����Ѵ�, �� ������ǥ�迡�� �浹ü���� �ø��� �����Ѵ�
	inline Frustum(float screenDepth, const Matrix4x4& matView, const Matrix4x4& matProj)
	{
		//�������� ����, ������������ �̿��� z-fighting�� ���̱����� ������������
		Matrix4x4 matLinearProj = matProj;
		float zMinimum = -matLinearProj[3].GetZ() / matLinearProj[2].GetZ();
		float r = screenDepth / (screenDepth - zMinimum);
		matLinearProj[2].SetZ(r);										//	f / (f - n)
		matLinearProj[3].SetZ(-r / zMinimum);							//-nf / (f - n)

		//������ķκ��� ����� ������Ų��, ��ȯ�� ���������� NDC��ǥ��� �����Ƿ�
		//�̸� ������ ����� ��ǥ����� ����ü�� ��ȯ��Ų��
		Matrix4x4 matFinalTranspose = (matView * matLinearProj).Transpose();
		m_planes[0] = matFinalTranspose[3] + matFinalTranspose[0];		//Left		(col3 + col0)
		m_planes[1] = matFinalTranspose[3] - matFinalTranspose[0];		//Right		(col3 - col0)
		m_planes[2] = matFinalTranspose[3] - matFinalTranspose[1];		//Up		(col3 - col1)
		m_planes[3] = matFinalTranspose[3] + matFinalTranspose[1];		//Bottom	(col3 + col1)
		m_planes[4] = matFinalTranspose[3] + matFinalTranspose[2];		//Near		(col3 + col2)
		m_planes[5] = matFinalTranspose[3] - matFinalTranspose[2];		//Far		(col3 - col2)
	}
	const Plane* GetPlanes() const { return m_planes; }
private:
	Plane m_planes[6];	//L, R, U, B, N, F
};