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
	if(!m_Direct3D)
	{
		return false;
	}

	// Direct3D 객체 초기화
	if(!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
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

	// 카메라 포지션 설정, 임의의위치에서 0.0을 바라보게한다
	XMVECTOR campos = XMVectorSet(0.0f, 2, -3, 1.0f);
	m_Camera->SetPosition(XMVectorGetX(campos), XMVectorGetY(campos), XMVectorGetZ(campos));
	XMVECTOR vLookAT = XMVectorSubtract(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), campos);
	vLookAT = XMVector3Normalize(vLookAT);
	float x = XMVectorGetX(vLookAT);
	float y = XMVectorGetY(vLookAT);
	float z = XMVectorGetZ(vLookAT);
	//xz기반으로 yaw를구한다
	//atan2f(회전하려는방향축의값, 기준축의값)
	float yaw = atan2f(x, z) * 180.0f / XM_PI; //yaw는 z->x이므로 부호는바뀌지않는다
	float pitch = atan2f(-y, sqrt(x * x + z * z)) * 180.0f / XM_PI; //same asinf(-y) pitch는y->z->-y인데 기준축이 z이므로 -y로가야한다, atan2의 sqrt는 수평축을의미한다
	//degree를받는다
	m_Camera->SetRotation(pitch, yaw, 0.0f);

	// m_Model 객체 생성
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// m_Model 객체 초기화
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), "../RasterTek07/data/teapot.txt", L"../RasterTek07/data/seafloor.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// m_LightShader 객체 생성
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// m_LightShader 객체 초기화
	if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// m_Light 객체 생성
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// m_Light 객체 초기화
	m_Light->SetDiffuseColor(1.0f, 0.3f, 0.7f, 1.0f);
	m_Light->SetDirection(0.0f, -0.5f, 1.0f);

	return true;
}


void GraphicsClass::Shutdown()
{
	// m_Light 객체 반환
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// m_LightShader 객체 반환
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// m_Model 객체 반환
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
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
	static float rotation = 0.0f;

	// 각 프레임의 rotation 변수를 업데이트합니다.
	rotation += (float)XM_PI * 0.001f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	
	// 그래픽 랜더링 처리
	return Render(rotation);
}


bool GraphicsClass::Render(float rotation)
{
	// 씬을 그리기 위해 버퍼를 지웁니다
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 삼각형이 회전 할 수 있도록 회전 값으로 월드 행렬을 회전합니다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Light 쉐이더를 사용하여 모델을 렌더링합니다.
	if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
								   m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor()))
	{
		return false;
	}

	// 버퍼의 내용을 화면에 출력합니다
	m_Direct3D->EndScene();

	return true;
}