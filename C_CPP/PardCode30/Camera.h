#pragma once
#include "CommonHeader.h"
struct CameraProperties
{
	//카메라의 속성
	Vector3 m_vPosition;			//카메라의 위치
	Vector3 m_vLookAt;				//카메라가 보는 위치

	Vector3 m_vRight;				//카메라의 우측벡터		(right)
	Vector3 m_vUp;					//카메라의 업벡터			(up)
	Vector3 m_vForward;				//카메라의 정면벡터		(forward)

	//사원수를 추가해야 오일러각<->사원수<->벡터 간의 원활한 각도계산이 가능 추후에 추가
	float m_fRoll;					
	float m_fPitch;
	float m_fYaw;

	float m_fFov;					//FieldOfView, 시야각(기준점이되는 Vertical)
	float m_fScreenWidth;			//화면너비
	float m_fScreenHeight;			//화면높이
	//float m_fAspectRatio;			//화면비, (화면너비 / 높이), fov와같이사용되어 투영행렬에 이용됨
	float m_fNearZ;					//근단면
	float m_fFarZ;					//원단면

	float m_fSpeedMove;				//이동속도
	float m_fSpeedRotate;			//회전속도
	CameraProperties() :
		m_vPosition({ 0.0f, 0.0f, 0.0f }),
		m_vLookAt({ 0.0f, 0.0f, 0.0f }),
		m_vRight({ 1.0f, 0.0f, 0.0f }),
		m_vUp({ 0.0f, 1.0f, 0.0f }),
		m_vForward({ 0.0f, 0.0f, 1.0f }),
		m_fRoll(0.0f),
		m_fPitch(0.0f),
		m_fYaw(0.0f),
		m_fFov(60.0f),
		m_fScreenWidth(800.0f),
		m_fScreenHeight(600.0f),
		//m_fAspectRatio(800.0f / 600.0f),
		m_fNearZ(0.1f),
		m_fFarZ(4000.0f),
		m_fSpeedMove(500.0f),
		m_fSpeedRotate(200.0f)
	{}
};
class BaseCamera
{
protected:
	BaseCamera();
	BaseCamera(const BaseCamera&) = delete;				//복사생성자삭제
	BaseCamera& operator=(const BaseCamera&) = delete;	//복사대입연산자삭제
	BaseCamera(BaseCamera&&) = delete;					//이동생성자삭제
	BaseCamera& operator=(BaseCamera&&) = delete;		//이동대입연산자삭제
	CameraProperties m_Properties;
	Matrix4x4 m_MatWorld = GetMat_Identity();
	Matrix4x4 m_MatView = GetMat_Identity();
	Matrix4x4 m_MatProj = GetMat_Identity();
	Matrix4x4 m_MatOrtho = GetMat_Identity();
	Matrix4x4 m_MatScreenSpace = GetMat_Identity();
	bool m_bDirtyFlag_View = true;
	bool m_bDirtyFlag_Proj = true;
public:
	//행렬
	const Matrix4x4& GetWorldMatrix();
	const Matrix4x4& GetViewMatrix();
	const Matrix4x4& GetProjMatrix();
	const Matrix4x4& GetOrthoMatrix();
	const Matrix4x4& GetScreenSpaceMatrix();

	//카메라속성
	const Vector3& GetPosition();
	const Vector3& GetTarget();
	const float GetMoveSpeed();
	void SetPosition(const Vector3& pos);
	void SetTarget(const Vector3& target);
	void MovePosition(const Vector3& translation);
	void UpdateEulerRotate(int deltaX, int deltaY, float sensetivity = 0.1f);

	//투영속성
	void SetFOV(float Fov);
	void SetScreenWidth(float width);
	void SetScreenHeight(float height);
	//void SetAsepectRatio(float aspectRatio);
	void SetClipPlanes(float nearZ, float farZ);
	float GetNearZ();
	float GetFarZ();

	virtual ~BaseCamera();
};
inline BaseCamera::BaseCamera()
{
	std::cout << "Initialize : " << "BaseCamera" << " Class" << '\n';
}

inline BaseCamera::~BaseCamera()
{
	std::cout << "Release : " << "BaseCamera" << " Class" << '\n';
}

inline const Matrix4x4& BaseCamera::GetWorldMatrix()
{
	GetViewMatrix();
	return m_MatWorld;
}
inline const Matrix4x4& BaseCamera::GetViewMatrix()
{
	if (m_bDirtyFlag_View)
	{
		//오일러각에의한 행렬갱신
		m_MatView = GetMat_ViewMatrix(m_Properties.m_vPosition, m_Properties.m_fPitch, m_Properties.m_fYaw, m_Properties.m_fRoll);
		m_MatWorld = GetMat_Inverse(m_MatView);
		m_Properties.m_vRight = m_MatWorld[0].ToVector3();		//u
		m_Properties.m_vUp = m_MatWorld[1].ToVector3();			//v
		m_Properties.m_vForward = m_MatWorld[2].ToVector3();	//w
		m_Properties.m_vLookAt = m_Properties.m_vForward * 1000000.0f;
		m_bDirtyFlag_View = false;
	}
	return m_MatView;
}

inline const Matrix4x4& BaseCamera::GetProjMatrix()
{
	if (m_bDirtyFlag_Proj)
	{
		m_MatProj = GetMat_Perspective(m_Properties.m_fScreenWidth / m_Properties.m_fScreenHeight, m_Properties.m_fFov, m_Properties.m_fNearZ, m_Properties.m_fFarZ);
		m_bDirtyFlag_Proj = false;
	}
	return m_MatProj;
}
inline const Matrix4x4& BaseCamera::GetOrthoMatrix()
{
	return GetMat_Orthographic_OffCenter(0.0f, m_Properties.m_fScreenWidth, 0.0f, m_Properties.m_fScreenHeight, m_Properties.m_fNearZ, m_Properties.m_fFarZ);
	//return GetMat_Orthographic(m_Properties.m_fScreenWidth, m_Properties.m_fScreenHeight, m_Properties.m_fNearZ, m_Properties.m_fFarZ);
}
inline const Vector3& BaseCamera::GetPosition()
{
	return m_Properties.m_vPosition;
}

inline const Vector3& BaseCamera::GetTarget()
{
	return m_Properties.m_vLookAt;
}
inline const float BaseCamera::GetMoveSpeed()
{
	return m_Properties.m_fSpeedMove;
}
inline void BaseCamera::SetPosition(const Vector3& pos)
{
	m_Properties.m_vPosition = pos;
	m_bDirtyFlag_View = true;
}
inline void BaseCamera::SetTarget(const Vector3& target)
{
	/*
	vLookat을 구했다면 여기서 역산하여 오일러각을 구할 수 있다 기본적으로 벡터의 구성요소인 x,y,z에서 역산한다
	atan2f, 즉 삼각함수의 tan값의역함수인 아크탄젠트를 이용해서 구한다, 이때 atan2f(_y, _x)는 (회전하려는축, 기준축) 이되고 이때의 요소는 이일반적인 삼각함수의회전에서(대변(회전하려는 축, y를의미), 인접변(기존축, 수평축, x를의미)) 한다
	
	1)yaw회전은 pitch회전에 의해서 성분요소인 x,z의 값이 둘 다 영향을 받으므로 비례값인 tan의 결과값이 바뀌지 않는다, 즉 atan2f의 비례값은 그대로 이기때문에 요소를 이용한다, 이때 yaw는 z->x회전이고 기본적인 요소값은 바뀌지 않는상태이므로 양수->양수 기준이기때문에
	atan2f(x, z)가 된다
	
	2)pitch회전또한 atan2f로 구할 순 있다, 이때 yaw에 영향을 받는데 차이점은 yaw에 의해 z는 그 요소가 변하나 y는 영향을 받지 않으므로 올바른 비례식을 그대로 사용 할 수 없다, 그러므로 벡터(x, y, z)를 xz평면에 투영하면 수평축에대한 길이요소값을 얻을 수 있으므로 수평축의 길이값
	sqrt(x^2 + z^2) 을 이용한다, 즉 수평축이 되며 수직축은 y인데 기본적인 pitch는 y -> z회전 이나 z축이 기준이되면 z -> -y로의 회전이 되므로
	atan2f(-y, sqrt(x^2 + z^2)) 가 된다, 하지만 비례식의 특성상 빗변의 길이가 1인 단위벡터에서 sinf는 빗변의 길이가 1일때 y의 이동량을 의미하므로 이에대한 역함수 asinf를 이용하면 각도가 나온다, 마찬가지로 -y로 회전하므로 asinf(-y)로 대체가 가능하며 이쪽이 효율적이다, 다만
	asinf의 결과는 -90~90 이나 일반적인 pitch는 이로충분하다, 이를 넘는 피치회전을 체크할경우 atan2f를 이용한다
	
	3)roll은 일반적으로 체크하지 않기에 0.0f로둔다	
	*/
	m_Properties.m_vLookAt = target;
	//forward계산
	Vector3 vLookAt = (target - m_Properties.m_vPosition).Normalize();
	//forward로 부터 yaw계산
	float forwardX = vLookAt.GetX();
	float forwardY = vLookAt.GetY();
	float forwardZ = vLookAt.GetZ();

	//atan2f(대변(회전의방향, 수직축), 인접변(기존방향, 수평축)) , yaw는 z->x를 의미하므로 atan2f(x, z)가된다
	//단위원과 반대방향이므로 인자를반대로
	m_Properties.m_fYaw = _RADTODEG(atan2f(forwardX, forwardZ));

	//단위벡터는 빗변의 길이가 1이므로 sin함수는 높이/빗변 이므로 아크사인에 높이를 집어넣으면 각이 나온다, 단위원과반대방향이므로음수
	m_Properties.m_fPitch = _RADTODEG(asinf(-forwardY));

	//roll이 되어있는지 여부는 up벡터에 따라 다르다, 일반적으로는 회전하지않으므로 0으로 가정한다
	m_Properties.m_fRoll = 0.0f;

	m_bDirtyFlag_View = true;
}
//방향벡터만큼 델타스칼라배한 벡터를 다 합해 최종 크기를 이동시킨다(선형독립)
inline void BaseCamera::MovePosition(const Vector3& translation)
{
	Vector3 vDeltaRight = m_Properties.m_vRight * translation.GetX();
	Vector3 vDeltaUp = m_Properties.m_vUp * translation.GetY();
	Vector3 vDeltaForward = m_Properties.m_vForward * translation.GetZ();
	m_Properties.m_vPosition += vDeltaRight + vDeltaUp + vDeltaForward;
	m_bDirtyFlag_View = true;
}
inline void BaseCamera::UpdateEulerRotate(int deltaX, int deltaY, float sensetivity)
{
	m_Properties.m_fYaw += deltaX * sensetivity;
	m_Properties.m_fPitch += deltaY * sensetivity;
	m_Properties.m_fPitch = _CLAMP(m_Properties.m_fPitch, -89.0f, 89.0f); //위아래각도제한
	m_bDirtyFlag_View = true;
}

inline void BaseCamera::SetFOV(float Fov)
{
	m_Properties.m_fFov = Fov;
	m_bDirtyFlag_Proj = true;
}
inline void BaseCamera::SetScreenWidth(float width)
{
	m_Properties.m_fScreenWidth = width;
	m_bDirtyFlag_Proj = true;
}
inline void BaseCamera::SetScreenHeight(float height)
{
	m_Properties.m_fScreenHeight = height;
	m_bDirtyFlag_Proj = true;
}
//inline void BaseCamera::SetAsepectRatio(float aspectRatio)
//{
//	m_Properties.m_fAspectRatio = aspectRatio;
//	m_bDirtyFlag_Proj = true;
//}
inline void BaseCamera::SetClipPlanes(float nearZ, float farZ)
{
	m_Properties.m_fNearZ = nearZ;
	m_Properties.m_fFarZ = farZ;
	m_bDirtyFlag_Proj = true;
}

inline float BaseCamera::GetNearZ()
{
	return m_Properties.m_fNearZ;
}

inline float BaseCamera::GetFarZ()
{
	return m_Properties.m_fFarZ;
}
