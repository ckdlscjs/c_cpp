#include "stdafx.h"
#include "PositionClass.h"


PositionClass::PositionClass()
{
}


PositionClass::PositionClass(const PositionClass& other)
{
}


PositionClass::~PositionClass()
{
}


void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
}


void PositionClass::GetRotationY(float& y)
{
	y = m_rotationY;
}

void PositionClass::GetRotationX(float& x)
{
	x = m_rotationX;
}


void PositionClass::TurnLeft(bool keydown)
{
	// Ű�� ������ ī�޶� �������� ���ƴ� �ӵ��� �����մϴ�. ȸ�� �ӵ��� ������ ������.
	if (keydown)
	{
		m_leftTurnSpeed += m_frameTime * 0.01f;

		if (m_leftTurnSpeed > (m_frameTime * 0.15f))
		{
			m_leftTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime* 0.005f;

		if (m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

	// ȸ�� �ӵ��� ����Ͽ� ȸ���� ������Ʈ�մϴ�.
	m_rotationY -= m_leftTurnSpeed;
	if (m_rotationY < 0.0f)
	{
		m_rotationY += 360.0f;
	}
}


void PositionClass::TurnRight(bool keydown)
{
	// Ű�� ������ ī�޶� ���������� ȸ���ϴ� �ӵ��� �����մϴ�. ȸ�� �ӵ��� ������ ������.
	if (keydown)
	{
		m_rightTurnSpeed += m_frameTime * 0.01f;

		if (m_rightTurnSpeed > (m_frameTime * 0.15f))
		{
			m_rightTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime* 0.005f;

		if (m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	// ȸ�� �ӵ��� ����Ͽ� ȸ���� ������Ʈ�մϴ�.
	m_rotationY += m_rightTurnSpeed;
	if (m_rotationY > 360.0f)
	{
		m_rotationY -= 360.0f;
	}
}

void PositionClass::TurnUp(bool keydown)
{
	// Ű�� ������ ī�޶� �������� ���ƴ� �ӵ��� �����մϴ�. ȸ�� �ӵ��� ������ ������.
	if (keydown)
	{
		m_UpTurnSpeed += m_frameTime * 0.01f;

		if (m_UpTurnSpeed > (m_frameTime * 0.15f))
		{
			m_UpTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_UpTurnSpeed -= m_frameTime * 0.005f;

		if (m_UpTurnSpeed < 0.0f)
		{
			m_UpTurnSpeed = 0.0f;
		}
	}

	// ȸ�� �ӵ��� ����Ͽ� ȸ���� ������Ʈ�մϴ�.
	m_rotationX -= m_UpTurnSpeed;
	if (m_rotationX < 0.0f)
	{
		m_rotationX += 360.0f;
	}
}

void PositionClass::TurnDown(bool keydown)
{
	// Ű�� ������ ī�޶� �������� ���ƴ� �ӵ��� �����մϴ�. ȸ�� �ӵ��� ������ ������.
	if (keydown)
	{
		m_DownTurnSpeed += m_frameTime * 0.01f;

		if (m_DownTurnSpeed > (m_frameTime * 0.15f))
		{
			m_DownTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_DownTurnSpeed -= m_frameTime * 0.005f;

		if (m_DownTurnSpeed < 0.0f)
		{
			m_DownTurnSpeed = 0.0f;
		}
	}

	// ȸ�� �ӵ��� ����Ͽ� ȸ���� ������Ʈ�մϴ�.
	m_rotationX += m_DownTurnSpeed;
	if (m_rotationX > 360.0f)
	{
		m_rotationX -= 360.0f;
	}
}
