#pragma once
#include "std.h"
struct CameraProperties
{
	//카메라의 속성
	XMFLOAT3 m_vPosition;			//카메라의 위치
	XMFLOAT3 m_vLookAt;				//카메라가 시선방향

	XMFLOAT3 m_vRight;				//카메라의 우측벡터		(right)
	XMFLOAT3 m_vUp;					//카메라의 업벡터			(up)
	XMFLOAT3 m_vForward;			//카메라의 정면벡터		(forward)

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
	XMMATRIX m_MatWorld = GetMat_Identity();
	XMMATRIX m_MatView = GetMat_Identity();
	XMMATRIX m_MatProj = GetMat_Identity();
	bool m_bDirtyFlag_View = true;
	bool m_bDirtyFlag_Proj = true;
public:
	//행렬
	const XMMATRIX& GetWorldMatrix();
	const XMMATRIX& GetViewMatrix();
	const XMMATRIX& GetProjMatrix();

	//카메라속성
	const XMFLOAT3& GetPosition();
	const XMFLOAT3& GetTarget();
	void SetPosition(const XMFLOAT3& pos);
	void SetTarget(const XMFLOAT3& targetPos);
	void MovePosition(const XMFLOAT3& translation);
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

inline const XMMATRIX& BaseCamera::GetWorldMatrix()
{
	return m_MatWorld;
}
inline const XMMATRIX& BaseCamera::GetViewMatrix()
{
	if (m_bDirtyFlag_View)
	{
		//오일러각에의한 행렬갱신
		m_MatView = GetMat_ViewMatrix(m_Properties.m_vPosition, m_Properties.m_fPitch, m_Properties.m_fYaw, m_Properties.m_fRoll);
		m_MatWorld = GetMat_Inverse(m_MatView);
		XMStoreFloat3(&m_Properties.m_vRight, m_MatWorld.r[0]);
		XMStoreFloat3(&m_Properties.m_vUp, m_MatWorld.r[1]);
		XMStoreFloat3(&m_Properties.m_vForward, m_MatWorld.r[2]);
		m_Properties.m_vLookAt = m_Properties.m_vForward;
		
		m_bDirtyFlag_View = false;
	}
	return m_MatView;
}

inline const XMMATRIX& BaseCamera::GetProjMatrix()
{
	if (m_bDirtyFlag_Proj)
	{
		m_MatProj = GetMat_Perspective(m_Properties.m_fAspectRatio, m_Properties.m_fFov, m_Properties.m_fNearZ, m_Properties.m_fFarZ);
		m_bDirtyFlag_Proj = false;
	}
	return m_MatProj;
}
inline const XMFLOAT3& BaseCamera::GetPosition()
{
	return m_Properties.m_vPosition;
}

inline const XMFLOAT3& BaseCamera::GetTarget()
{
	return m_Properties.m_vLookAt;
}
inline void BaseCamera::SetPosition(const XMFLOAT3& pos)
{
	m_Properties.m_vPosition = pos;
	m_bDirtyFlag_View = true;
}
inline void BaseCamera::SetTarget(const XMFLOAT3& targetPos)
{
	//forward계산
	XMVECTOR vPos = XMLoadFloat3(&m_Properties.m_vPosition);
	XMVECTOR vTarget = XMLoadFloat3(&targetPos);
	XMVECTOR vLookAt = XMVector3Normalize(vTarget - vPos);
	XMStoreFloat3(&m_Properties.m_vLookAt, vLookAt);

	//forward로 부터 yaw계산
	float forwardX = XMVectorGetX(vLookAt);
	float forwardY = XMVectorGetY(vLookAt);
	float forwardZ = XMVectorGetZ(vLookAt);
	//yaw는 시계방향 회전이 되므로(왼손좌표계), atan2는 반시계의 회전각을 의미하기에 평면을 반대로 넣어준다(result -> radian, need to convert degree)
	m_Properties.m_fYaw = _RADTODEG(atan2f(forwardX, forwardZ));
	
	//단위벡터는 빗변의 길이가 1이므로 sin함수는 높이/빗변 이므로 아크사인에 높이를 집어넣으면 각이 나온다
	m_Properties.m_fPitch = _RADTODEG(asinf(forwardY));

	//roll이 되어있는지 여부는 up벡터에 따라 다르다, 일반적으로는 회전하지않으므로 0으로 가정한다
	m_Properties.m_fRoll = 0.0f;

	m_bDirtyFlag_View = true;
}
inline void BaseCamera::MovePosition(const XMFLOAT3& translation)
{
	XMFLOAT3 vDeltaRight(m_Properties.m_vRight.x * translation.x, m_Properties.m_vRight.y * translation.x, m_Properties.m_vRight.z * translation.x);
	XMFLOAT3 vDeltaUp(m_Properties.m_vUp.x*translation.y, m_Properties.m_vUp.y * translation.y, m_Properties.m_vUp.z * translation.y);
	XMFLOAT3 vDeltaForward(m_Properties.m_vForward.x * translation.z, m_Properties.m_vForward.y * translation.z, m_Properties.m_vForward.z * translation.z);
	m_Properties.m_vPosition.x += vDeltaRight.x + vDeltaUp.x + vDeltaForward.x;
	m_Properties.m_vPosition.y += vDeltaRight.y + vDeltaUp.y + vDeltaForward.y;
	m_Properties.m_vPosition.z += vDeltaRight.z + vDeltaUp.z + vDeltaForward.z;
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