#pragma once
#include "std.h"
struct CameraProperties
{
	//카메라의 속성
	Position m_vPosition;			//카메라의 위치
	Position m_vLookAt;				//카메라의 시선

	Vector3 m_vRight;				//카메라의 우측벡터		(right)
	Vector3 m_vUp;					//카메라의 업벡터			(up)
	Vector3 m_vForward;				//카메라의 정면벡터		(forward)

	//사원수를 추가해야 오일러각<->사원수<->벡터 간의 원활한 각도계산이 가능 추후에 추가
	float m_fRoll;					
	float m_fPitch;
	float m_fYaw;

	float m_fFov;					//FieldOfView, 시야각(기준점이되는 Vertical)
	float m_fAspectRatio;			//화면비, (화면너비 / 높이), fov와같이사용되어 투영행렬에 이용됨
	float m_fNearZ;					//근단면
	float m_fFarZ;					//원단면
	CameraProperties() :
		m_vPosition({ 0.0f, 0.0f, 0.0f }),
		m_vLookAt({ 0.0f, 0.0f, 0.0f }),
		m_vRight({1.0f, 0.0f, 0.0f}),
		m_vUp({ 0.0f, 1.0f, 0.0f }),
		m_vForward({0.0f, 0.0f, 1.0f}),
		m_fRoll(0.0f),
		m_fPitch(0.0f),
		m_fYaw(0.0f),
		m_fFov(60.0f),
		m_fAspectRatio(800.0f / 600.0f),
		m_fNearZ(0.1f),
		m_fFarZ(1000.0f)
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
	bool m_bDirtyFlag_View = true;
	bool m_bDirtyFlag_Proj = true;
public:
	//행렬
	const Matrix4x4& GetWorldMatrix();
	const Matrix4x4& GetViewMatrix();
	const Matrix4x4& GetProjMatrix();

	//카메라속성
	const Position& GetPosition();
	const Position& GetTarget();
	void SetPosition(const Position& pos);
	void SetTarget(const Position& target);
	void MovePosition(const Vector3& translation);
	void UpdateEulerRotate(int deltaX, int deltaY, float sensetivity = 0.1f);

	//투영속성
	void SetFOV(float Fov);
	void SetAsepectRatio(float aspectRatio);
	void SetClipPlanes(float nearZ, float farZ);

	virtual void Frame(float detlaTime) = 0;
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
	return m_MatWorld;
}
inline const Matrix4x4& BaseCamera::GetViewMatrix()
{
	if (m_bDirtyFlag_View)
	{
		//오일러각에의한 행렬갱신
		m_MatView = GetMat_ViewMatrix(m_Properties.m_vPosition, m_Properties.m_fPitch, m_Properties.m_fYaw, m_Properties.m_fRoll);
		m_MatWorld = GetMat_Inverse(m_MatView);
		m_Properties.m_vRight = m_MatWorld.r[0].ToVector3();
		m_Properties.m_vUp = m_MatWorld.r[1].ToVector3();
		m_Properties.m_vForward = m_MatWorld.r[2].ToVector3();
		m_Properties.m_vLookAt = m_Properties.m_vForward;
		
		m_bDirtyFlag_View = false;
	}
	return m_MatView;
}

inline const Matrix4x4& BaseCamera::GetProjMatrix()
{
	if (m_bDirtyFlag_Proj)
	{
		m_MatProj = GetMat_Perspective(m_Properties.m_fAspectRatio, m_Properties.m_fFov, m_Properties.m_fNearZ, m_Properties.m_fFarZ);
		m_bDirtyFlag_Proj = false;
	}
	return m_MatProj;
}
inline const Position& BaseCamera::GetPosition()
{
	return m_Properties.m_vPosition;
}

inline const Position& BaseCamera::GetTarget()
{
	return m_Properties.m_vLookAt;
}
inline void BaseCamera::SetPosition(const Position& pos)
{
	m_Properties.m_vPosition = pos;
	m_bDirtyFlag_View = true;
}
inline void BaseCamera::SetTarget(const Position& target)
{
	//forward계산
	Vector3 vLookAt = (m_Properties.m_vPosition - target).Normalize();
	m_Properties.m_vLookAt = vLookAt;

	//forward로 부터 yaw계산
	float forwardX = vLookAt.GetX();
	float forwardY = vLookAt.GetY();
	float forwardZ = vLookAt.GetZ();
	//yaw는 시계방향 회전이 되므로(왼손좌표계), atan2는 반시계의 회전각을 의미하기에 평면을 반대로 넣어준다(result -> radian, need to convert degree)
	m_Properties.m_fYaw = _RADTODEG(atan2f(forwardX, forwardZ));
	
	//단위벡터는 빗변의 길이가 1이므로 sin함수는 높이/빗변 이므로 아크사인에 높이를 집어넣으면 각이 나온다
	m_Properties.m_fPitch = _RADTODEG(asinf(forwardY));

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
inline void BaseCamera::SetAsepectRatio(float aspectRatio)
{
	m_Properties.m_fAspectRatio = aspectRatio;
	m_bDirtyFlag_Proj = true;
}
inline void BaseCamera::SetClipPlanes(float nearZ, float farZ)
{
	m_Properties.m_fNearZ = nearZ;
	m_Properties.m_fFarZ = farZ;
	m_bDirtyFlag_Proj = true;
}