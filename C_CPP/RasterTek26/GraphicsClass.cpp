#include "stdafx.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "textureshaderclass.h"
#include "transparentshaderclass.h"
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
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
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

	// 첫 번째 모델 객체를 만듭니다.
	m_Model1 = new ModelClass;
	if(!m_Model1)
	{
		return false;
	}

	// 첫 번째 모델 객체를 초기화합니다.
	if(!m_Model1->Initialize(m_Direct3D->GetDevice(), L"../RasterTek26/data/dirt01.dds", "../RasterTek26/data/square.txt"))
	{
		MessageBox(hwnd, L"Could not initialize the first model object.", L"Error", MB_OK);
		return false;
	}

	// 두 번째 모델 객체를 만듭니다.
	m_Model2 = new ModelClass;
	if(!m_Model2)
	{
		return false;
	}

	// 두 번째 모델 객체를 초기화합니다.
	if(!m_Model2->Initialize(m_Direct3D->GetDevice(), L"../RasterTek26/data/stone01.dds", "../RasterTek26/data/square.txt"))
	{
		MessageBox(hwnd, L"Could not initialize the second model object.", L"Error", MB_OK);
		return false;
	}

	// 텍스처 쉐이더 객체를 생성한다.
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader)
	{
		return false;
	}

	// 텍스처 쉐이더 객체를 초기화한다.
	if(!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// 투명한 셰이더 개체를 만듭니다.
	m_TransparentShader = new TransparentShaderClass;
	if(!m_TransparentShader)
	{
		return false;
	}

	// 투명 쉐이더 객체를 초기화합니다.
	if(!m_TransparentShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the transparent shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// 투명 쉐이더 객체를 해제합니다.
	if (m_TransparentShader)
	{
		m_TransparentShader->Shutdown();
		delete m_TransparentShader;
		m_TransparentShader = 0;
	}

	// 텍스처 쉐이더 객체를 해제한다.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 두 번째 모델 객체를 해제합니다.
	if (m_Model2)
	{
		m_Model2->Shutdown();
		delete m_Model2;
		m_Model2 = 0;
	}

	// 첫 번째 모델 객체를 해제합니다.
	if (m_Model1)
	{
		m_Model1->Shutdown();
		delete m_Model1;
		m_Model1 = 0;
	}

	// m_Camera 객체 반환
	if (m_Camera)
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
	// 카메라 위치 설정
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	return true;
}


bool GraphicsClass::Render()
{
	// 블렌딩 양을 50% 설정합니다.
	float blendAmount = 0.5f;
	
	// 씬을 그리기 위해 버퍼를 지웁니다
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model1->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더로 모델을 렌더링한다.
	if(!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model1->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model1->GetTexture()))
	{
		return false;
	}

	// 하나의 단위로 오른쪽으로 번역하고 하나의 단위로 카메라로 번역합니다.
	worldMatrix = XMMatrixTranslation(1.0f, 0.0f, -1.0f); // 월드행렬을통해서 좌표계이동

	// 투명도가 작동하도록 알파 블렌드를 켭니다.
	m_Direct3D->TurnOnAlphaBlending();

	// 두 번째 사각형 모델을 그래픽 파이프 라인에 배치합니다.
	m_Model2->Render(m_Direct3D->GetDeviceContext());

	// 돌 텍스처로 두 번째 사각형 모델을 렌더링하고 투명도를 위해 50 %의 혼합량을 사용합니다.
	if(!m_TransparentShader->Render(m_Direct3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model2->GetTexture(), blendAmount))
	{
		return false;
	}

	// 알파 블렌딩을 끕니다.
	m_Direct3D->TurnOffAlphaBlending();

	// 버퍼의 내용을 화면에 출력합니다
	m_Direct3D->EndScene();

	return true;
}