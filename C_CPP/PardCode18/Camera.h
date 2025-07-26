#pragma once
#include "std.h"
struct CameraProperties
{
	//ī�޶��� �Ӽ�
	Position m_vPosition;			//ī�޶��� ��ġ
	Position m_vLookAt;				//ī�޶��� �ü�

	Vector3 m_vRight;				//ī�޶��� ��������		(right)
	Vector3 m_vUp;					//ī�޶��� ������			(up)
	Vector3 m_vForward;				//ī�޶��� ���麤��		(forward)

	//������� �߰��ؾ� ���Ϸ���<->�����<->���� ���� ��Ȱ�� ��������� ���� ���Ŀ� �߰�
	float m_fRoll;					
	float m_fPitch;
	float m_fYaw;

	float m_fFov;					//FieldOfView, �þ߰�(�������̵Ǵ� Vertical)
	float m_fAspectRatio;			//ȭ���, (ȭ��ʺ� / ����), fov�Ͱ��̻��Ǿ� ������Ŀ� �̿��
	float m_fNearZ;					//�ٴܸ�
	float m_fFarZ;					//���ܸ�
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
	BaseCamera(const BaseCamera&) = delete;				//��������ڻ���
	BaseCamera& operator=(const BaseCamera&) = delete;	//������Կ����ڻ���
	BaseCamera(BaseCamera&&) = delete;					//�̵������ڻ���
	BaseCamera& operator=(BaseCamera&&) = delete;		//�̵����Կ����ڻ���
	CameraProperties m_Properties;
	Matrix4x4 m_MatWorld = GetMat_Identity();
	Matrix4x4 m_MatView = GetMat_Identity();
	Matrix4x4 m_MatProj = GetMat_Identity();
	bool m_bDirtyFlag_View = true;
	bool m_bDirtyFlag_Proj = true;
public:
	//���
	const Matrix4x4& GetWorldMatrix();
	const Matrix4x4& GetViewMatrix();
	const Matrix4x4& GetProjMatrix();

	//ī�޶�Ӽ�
	const Position& GetPosition();
	const Position& GetTarget();
	void SetPosition(const Position& pos);
	void SetTarget(const Position& target);
	void MovePosition(const Vector3& translation);
	void UpdateEulerRotate(int deltaX, int deltaY, float sensetivity = 0.1f);

	//�����Ӽ�
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
		//���Ϸ��������� ��İ���
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
	//forward���
	Vector3 vLookAt = (m_Properties.m_vPosition - target).Normalize();
	m_Properties.m_vLookAt = vLookAt;

	//forward�� ���� yaw���
	float forwardX = vLookAt.GetX();
	float forwardY = vLookAt.GetY();
	float forwardZ = vLookAt.GetZ();
	//yaw�� �ð���� ȸ���� �ǹǷ�(�޼���ǥ��), atan2�� �ݽð��� ȸ������ �ǹ��ϱ⿡ ����� �ݴ�� �־��ش�(result -> radian, need to convert degree)
	m_Properties.m_fYaw = _RADTODEG(atan2f(forwardX, forwardZ));
	
	//�������ʹ� ������ ���̰� 1�̹Ƿ� sin�Լ��� ����/���� �̹Ƿ� ��ũ���ο� ���̸� ��������� ���� ���´�
	m_Properties.m_fPitch = _RADTODEG(asinf(forwardY));

	//roll�� �Ǿ��ִ��� ���δ� up���Ϳ� ���� �ٸ���, �Ϲ������δ� ȸ�����������Ƿ� 0���� �����Ѵ�
	m_Properties.m_fRoll = 0.0f;

	m_bDirtyFlag_View = true;
}
//���⺤�͸�ŭ ��Ÿ��Į����� ���͸� �� ���� ���� ũ�⸦ �̵���Ų��(��������)
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
	m_Properties.m_fPitch = _CLAMP(m_Properties.m_fPitch, -89.0f, 89.0f); //���Ʒ���������
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