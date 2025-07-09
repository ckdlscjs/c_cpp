#include "TempObj.h"
#include "InputSystem.h"

TempObj::TempObj()
{
	m_vScale = XMFLOAT3(200.0f, 200.0f, 200.0f);
	m_vRotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vTranslation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

TempObj::~TempObj()
{
}

void TempObj::Init()
{
}

void TempObj::Frame()
{
}

void TempObj::Render()
{
}

void TempObj::Release()
{
}
