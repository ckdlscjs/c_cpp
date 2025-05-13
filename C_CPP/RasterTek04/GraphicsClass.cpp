#include "stdafx.h"
#include "GraphicsClass.h"
#include "D3DClass.h"

GraphicsClass::GraphicsClass()
{
}

GraphicsClass::GraphicsClass(const GraphicsClass& graphicClass)
{
}

GraphicsClass::~GraphicsClass()
{
}
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	//D3D ��ü ����
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
		return false;
	
	//D3D ��ü �ʱ�ȭ
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could Not Initialize D3DClass", L"Error", MB_OK);
		return false;
	}
	return true;
}

void GraphicsClass::Shutdown()
{
	//D3D��ü ��ȯ
	if (m_Direct3D)
	{
		m_Direct3D->Shotdown();
		_aligned_free(m_Direct3D);
		m_Direct3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	//������ �Լ�ó��

	//Render�γѾ
	return Render();
}

bool GraphicsClass::Render()
{
	//������ �Լ�ó��

	//���� �׸��� ���� ���۸� �����
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 0.5f);

	//������ ������ ȭ�鿡 ����Ѵ�
	m_Direct3D->EndScene();
	return true;
}
