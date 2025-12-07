#include "stdafx.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"
#include "lightshaderclass.h"
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D 객체 생성
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	// Direct3D 객체 초기화
	bool result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// m_Camera 객체 생성
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// 모델 객체를 생성합니다.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	// 모델 객체를 초기화합니다.
	result = m_Model->Initialize(m_Direct3D->GetDevice(), L"../RasterTek30/data/stone01.dds", "../RasterTek30/data/plane01.txt");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 라이트 쉐이더 객체를 만듭니다.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	// 라이트 쉐이더 객체를 초기화합니다.
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// 첫 번째 조명 객체를 만듭니다.
	m_Light1 = new LightClass;
	if(!m_Light1)
	{
		return false;
	}

	// 첫 번째 조명 객체를초기화합니다.
	m_Light1->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_Light1->SetPosition(-3.0f, 1.0f, 3.0f);

	// 두 번째 조명 객체를 만듭니다.
	m_Light2 = new LightClass;
	if(!m_Light2)
	{
		return false;
	}

	// 두 번째 조명 객체를 초기화합니다.
	m_Light2->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
	m_Light2->SetPosition(3.0f, 1.0f, 3.0f);

	// 세 번째 조명 객체를 만듭니다.
	m_Light3 = new LightClass;
	if(!m_Light3)
	{
		return false;
	}

	// 세 번째 조명 객체를 초기화합니다.
	m_Light3->SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
	m_Light3->SetPosition(-3.0f, 1.0f, -3.0f);

	// 네 번째 조명 객체를 만듭니다.
	m_Light4 = new LightClass;
	if(!m_Light4)
	{
		return false;
	}

	// 네 번째 조명 객체를 초기화합니다.
	m_Light4->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light4->SetPosition(3.0f, 1.0f, -3.0f);

	return true;
}


void GraphicsClass::Shutdown()
{
	// 조명 객체들을 해제합니다.
	if(m_Light1)
	{
		delete m_Light1;
		m_Light1 = 0;
	}

	if(m_Light2)
	{
		delete m_Light2;
		m_Light2 = 0;
	}

	if(m_Light3)
	{
		delete m_Light3;
		m_Light3 = 0;
	}

	if(m_Light4)
	{
		delete m_Light4;
		m_Light4 = 0;
	}

	// 라이트 쉐이더 객체를 해제합니다.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// 모델 객체를 해제합니다.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// 카메라 객체를 해제합니다.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D 객체 반환
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}


bool GraphicsClass::Frame()
{
	// 카메라 위치를 설정합니다.
	m_Camera->SetPosition(0.0f, 2.0f, -12.0f);

	// 장면을 렌더링합니다.
	return Render();
}


bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMFLOAT4 diffuseColor[4];
	XMFLOAT4 lightPosition[4];
	bool result;


	// 4 개의 밝은 색상에서 확산 색상 배열을 만듭니다.
	diffuseColor[0] = m_Light1->GetDiffuseColor();
	diffuseColor[1] = m_Light2->GetDiffuseColor();
	diffuseColor[2] = m_Light3->GetDiffuseColor();
	diffuseColor[3] = m_Light4->GetDiffuseColor();

	// 네 개의 가벼운 위치에서 가벼운 위치 배열을 만듭니다.
	lightPosition[0] = m_Light1->GetPosition();
	lightPosition[1] = m_Light2->GetPosition();
	lightPosition[2] = m_Light3->GetPosition();
	lightPosition[3] = m_Light4->GetPosition();

	// 장면을 시작할 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 라이트 쉐이더와 광원 배열을 사용하여 모델을 렌더링합니다.
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
								   m_Model->GetTexture(), diffuseColor, lightPosition);
	if(!result)
	{
		return false;
	}

	// 렌더링 된 장면을 화면에 표시합니다.
	m_Direct3D->EndScene();

	return true;
}