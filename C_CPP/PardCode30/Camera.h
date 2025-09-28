#pragma once
#include "CommonHeader.h"
struct CameraProperties
{
	//ī�޶��� �Ӽ�
	Vector3 m_vPosition;			//ī�޶��� ��ġ
	Vector3 m_vLookAt;				//ī�޶� ���� ��ġ

	Vector3 m_vRight;				//ī�޶��� ��������		(right)
	Vector3 m_vUp;					//ī�޶��� ������			(up)
	Vector3 m_vForward;				//ī�޶��� ���麤��		(forward)

	//������� �߰��ؾ� ���Ϸ���<->�����<->���� ���� ��Ȱ�� ��������� ���� ���Ŀ� �߰�
	float m_fRoll;					
	float m_fPitch;
	float m_fYaw;

	float m_fFov;					//FieldOfView, �þ߰�(�������̵Ǵ� Vertical)
	float m_fScreenWidth;			//ȭ��ʺ�
	float m_fScreenHeight;			//ȭ�����
	//float m_fAspectRatio;			//ȭ���, (ȭ��ʺ� / ����), fov�Ͱ��̻��Ǿ� ������Ŀ� �̿��
	float m_fNearZ;					//�ٴܸ�
	float m_fFarZ;					//���ܸ�

	float m_fSpeedMove;				//�̵��ӵ�
	float m_fSpeedRotate;			//ȸ���ӵ�
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
	BaseCamera(const BaseCamera&) = delete;				//��������ڻ���
	BaseCamera& operator=(const BaseCamera&) = delete;	//������Կ����ڻ���
	BaseCamera(BaseCamera&&) = delete;					//�̵������ڻ���
	BaseCamera& operator=(BaseCamera&&) = delete;		//�̵����Կ����ڻ���
	CameraProperties m_Properties;
	Matrix4x4 m_MatWorld = GetMat_Identity();
	Matrix4x4 m_MatView = GetMat_Identity();
	Matrix4x4 m_MatProj = GetMat_Identity();
	Matrix4x4 m_MatOrtho = GetMat_Identity();
	Matrix4x4 m_MatScreenSpace = GetMat_Identity();
	bool m_bDirtyFlag_View = true;
	bool m_bDirtyFlag_Proj = true;
public:
	//���
	const Matrix4x4& GetWorldMatrix();
	const Matrix4x4& GetViewMatrix();
	const Matrix4x4& GetProjMatrix();
	const Matrix4x4& GetOrthoMatrix();
	const Matrix4x4& GetScreenSpaceMatrix();

	//ī�޶�Ӽ�
	const Vector3& GetPosition();
	const Vector3& GetTarget();
	const float GetMoveSpeed();
	void SetPosition(const Vector3& pos);
	void SetTarget(const Vector3& target);
	void MovePosition(const Vector3& translation);
	void UpdateEulerRotate(int deltaX, int deltaY, float sensetivity = 0.1f);

	//�����Ӽ�
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
		//���Ϸ��������� ��İ���
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
	vLookat�� ���ߴٸ� ���⼭ �����Ͽ� ���Ϸ����� ���� �� �ִ� �⺻������ ������ ��������� x,y,z���� �����Ѵ�
	atan2f, �� �ﰢ�Լ��� tan���ǿ��Լ��� ��ũź��Ʈ�� �̿��ؼ� ���Ѵ�, �̶� atan2f(_y, _x)�� (ȸ���Ϸ�����, ������) �̵ǰ� �̶��� ��Ҵ� ���Ϲ����� �ﰢ�Լ���ȸ������(�뺯(ȸ���Ϸ��� ��, y���ǹ�), ������(������, ������, x���ǹ�)) �Ѵ�
	
	1)yawȸ���� pitchȸ���� ���ؼ� ���п���� x,z�� ���� �� �� ������ �����Ƿ� ��ʰ��� tan�� ������� �ٲ��� �ʴ´�, �� atan2f�� ��ʰ��� �״�� �̱⶧���� ��Ҹ� �̿��Ѵ�, �̶� yaw�� z->xȸ���̰� �⺻���� ��Ұ��� �ٲ��� �ʴ»����̹Ƿ� ���->��� �����̱⶧����
	atan2f(x, z)�� �ȴ�
	
	2)pitchȸ������ atan2f�� ���� �� �ִ�, �̶� yaw�� ������ �޴µ� �������� yaw�� ���� z�� �� ��Ұ� ���ϳ� y�� ������ ���� �����Ƿ� �ùٸ� ��ʽ��� �״�� ��� �� �� ����, �׷��Ƿ� ����(x, y, z)�� xz��鿡 �����ϸ� �����࿡���� ���̿�Ұ��� ���� �� �����Ƿ� �������� ���̰�
	sqrt(x^2 + z^2) �� �̿��Ѵ�, �� �������� �Ǹ� �������� y�ε� �⺻���� pitch�� y -> zȸ�� �̳� z���� �����̵Ǹ� z -> -y���� ȸ���� �ǹǷ�
	atan2f(-y, sqrt(x^2 + z^2)) �� �ȴ�, ������ ��ʽ��� Ư���� ������ ���̰� 1�� �������Ϳ��� sinf�� ������ ���̰� 1�϶� y�� �̵����� �ǹ��ϹǷ� �̿����� ���Լ� asinf�� �̿��ϸ� ������ ���´�, ���������� -y�� ȸ���ϹǷ� asinf(-y)�� ��ü�� �����ϸ� ������ ȿ�����̴�, �ٸ�
	asinf�� ����� -90~90 �̳� �Ϲ����� pitch�� �̷�����ϴ�, �̸� �Ѵ� ��ġȸ���� üũ�Ұ�� atan2f�� �̿��Ѵ�
	
	3)roll�� �Ϲ������� üũ���� �ʱ⿡ 0.0f�εд�	
	*/
	m_Properties.m_vLookAt = target;
	//forward���
	Vector3 vLookAt = (target - m_Properties.m_vPosition).Normalize();
	//forward�� ���� yaw���
	float forwardX = vLookAt.GetX();
	float forwardY = vLookAt.GetY();
	float forwardZ = vLookAt.GetZ();

	//atan2f(�뺯(ȸ���ǹ���, ������), ������(��������, ������)) , yaw�� z->x�� �ǹ��ϹǷ� atan2f(x, z)���ȴ�
	//�������� �ݴ�����̹Ƿ� ���ڸ��ݴ��
	m_Properties.m_fYaw = _RADTODEG(atan2f(forwardX, forwardZ));

	//�������ʹ� ������ ���̰� 1�̹Ƿ� sin�Լ��� ����/���� �̹Ƿ� ��ũ���ο� ���̸� ��������� ���� ���´�, ���������ݴ�����̹Ƿ�����
	m_Properties.m_fPitch = _RADTODEG(asinf(-forwardY));

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
