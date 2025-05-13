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
	//D3D 객체 생성
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
		return false;
	
	//D3D 객체 초기화
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could Not Initialize D3DClass", L"Error", MB_OK);
		return false;
	}
	return true;
}

void GraphicsClass::Shutdown()
{
	//D3D객체 반환
	if (m_Direct3D)
	{
		m_Direct3D->Shotdown();
		_aligned_free(m_Direct3D);
		m_Direct3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	//프레임 함수처리

	//Render로넘어감
	return Render();
}

bool GraphicsClass::Render()
{
	//렌더링 함수처리

	//씬을 그리기 위해 버퍼를 지운다
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 0.5f);

	//버퍼의 내용을 화면에 출력한다
	m_Direct3D->EndScene();
	return true;
}
