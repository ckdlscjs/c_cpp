#include "stdafx.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"
#include "projectionshaderclass.h"
#include "textureclass.h"
#include "viewpointclass.h"
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
	if(!m_Direct3D)
	{
		return false;
	}

	// Direct3D 객체 초기화
	bool result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
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

	// 카메라의 초기 위치와 회전을 설정합니다.
	m_Camera->SetPosition(XMFLOAT3(0.0f, 7.0f, -10.0f));
	m_Camera->SetRotation(XMFLOAT3(35.0f, 0.0f, 0.0f));
	
	// 그라운드 모델 객체를 만듭니다.
	m_GroundModel = new ModelClass;
	if(!m_GroundModel)
	{
		return false;
	}

	// 지면 모델 객체를 초기화합니다.
	result = m_GroundModel->Initialize(m_Direct3D->GetDevice(), "../Dx11Demo_44/data/floor.txt", L"../Dx11Demo_44/data/stone.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}


	// 큐브 모델 객체를 생성합니다.
	m_CubeModel = new ModelClass;
	if(!m_CubeModel)
	{
		return false;
	}

	// 큐브 모델 객체를 초기화 합니다.
	result = m_CubeModel->Initialize(m_Direct3D->GetDevice(), "../Dx11Demo_44/data/cube.txt", L"../Dx11Demo_44/data/seafloor.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}

	// 조명 객체를 생성합니다.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// 조명 객체를 초기화 합니다.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetPosition(2.0f, 5.0f, -2.0f);

	// 프로젝션 셰이더 개체를 만듭니다.
	m_ProjectionShader = new ProjectionShaderClass;
	if(!m_ProjectionShader)
	{
		return false;
	}

	// 프로젝션 셰이더 개체를 초기화합니다.
	result = m_ProjectionShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the projection shader object.", L"Error", MB_OK);
		return false;
	}

	// 투영 텍스처 객체를 만듭니다.
	m_ProjectionTexture = new TextureClass;
	if(!m_ProjectionTexture)
	{
		return false;
	}

	// 투영 텍스처 객체를 초기화합니다.
	result = m_ProjectionTexture->Initialize(m_Direct3D->GetDevice(), L"../Dx11Demo_44/data/grate.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the projection texture object.", L"Error", MB_OK);
		return false;
	}

	// 뷰 포인트 객체를 만듭니다.
	m_ViewPoint = new ViewPointClass;
	if(!m_ViewPoint)
	{
		return false;
	}

	// 뷰 포인트 객체를 초기화합니다.
	m_ViewPoint->SetPosition(2.0f, 5.0f, -2.0f);
	m_ViewPoint->SetLookAt(0.0f, 0.0f, 0.0f);
	m_ViewPoint->SetProjectionParameters((float)(XM_PI / 2.0f), 1.0f, 0.1f, 100.0f);
	m_ViewPoint->GenerateViewMatrix();
	m_ViewPoint->GenerateProjectionMatrix();

	return true;
}


void GraphicsClass::Shutdown()
{
	// 뷰 포인트 객체를 해제합니다.
	if(m_ViewPoint)
	{
		delete m_ViewPoint;
		m_ViewPoint = 0;
	}

	// 투영 텍스처 객체를 해제합니다.
	if(m_ProjectionTexture)
	{
		m_ProjectionTexture->Shutdown();
		delete m_ProjectionTexture;
		m_ProjectionTexture = 0;
	}

	// 투영 쉐이더 객체를 해제합니다.
	if(m_ProjectionShader)
	{
		m_ProjectionShader->Shutdown();
		delete m_ProjectionShader;
		m_ProjectionShader = 0;
	}

	// light 오브젝트를 해제한다.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// 큐브 모델 오브젝트를 해제하십시오.
	if(m_CubeModel)
	{
		m_CubeModel->Shutdown();
		delete m_CubeModel;
		m_CubeModel = 0;
	}

	// 그라운드 모델 객체를 해제한다.
	if(m_GroundModel)
	{
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
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
	// 그래픽 장면을 렌더링합니다.
	return Render();
}


bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX viewMatrix2, projectionMatrix2;
	
	// 씬을 그리기 위해 버퍼를 지웁니다
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 뷰 포인트 객체에서 뷰 및 투영 행렬을 가져옵니다.
	m_ViewPoint->GetViewMatrix(viewMatrix2);
	m_ViewPoint->GetProjectionMatrix(projectionMatrix2);

	// ground 모델에 대한 번역을 설정합니다.
	worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	
	// 투영 셰이더를 사용하여 지상 모델을 렌더링합니다.
	m_GroundModel->Render(m_Direct3D->GetDeviceContext());
	
	if(!m_ProjectionShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_GroundModel->GetTexture(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(),
		viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture()))
	{
		return false;
	}

	// 월드 행렬을 재설정하고 큐브 모델에 대한 변환을 설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	// 프로젝션 셰이더를 사용하여 큐브 모델을 렌더링합니다.
	m_CubeModel->Render(m_Direct3D->GetDeviceContext());

	if(!m_ProjectionShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_CubeModel->GetTexture(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetPosition(),
		viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture()))
	{
		return false;
	}
							   
	// 렌더링 된 장면을 화면에 표시합니다.
	m_Direct3D->EndScene();

	return true;
}