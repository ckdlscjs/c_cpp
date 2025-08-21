#pragma once
#include "Plane.h"

__declspec(align(16))
class Frustum
{
public:
	inline Frustum() = default;
	//v * p 기반으로 절두체를 역산한다, 즉 월드좌표계에서 충돌체계산및 컬링을 수행한다
	inline Frustum(float screenDepth, const Matrix4x4& matView, const Matrix4x4& matProj)
	{
		//렌더링과 별도, 선형적분포를 이용해 z-fighting을 줄이기위한 깊이재분포계산
		Matrix4x4 matLinearProj = matProj;
		float zMinimum = -matLinearProj[3].GetZ() / matLinearProj[2].GetZ();
		float r = screenDepth / (screenDepth - zMinimum);
		matLinearProj[2].SetZ(r);										//	f / (f - n)
		matLinearProj[3].SetZ(-r / zMinimum);							//-nf / (f - n)

		//투영행렬로부터 평면을 구성시킨다, 변환된 최종정점이 NDC좌표계상에 있으므로
		//이를 역산해 계산할 좌표계상의 절두체로 변환시킨다
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