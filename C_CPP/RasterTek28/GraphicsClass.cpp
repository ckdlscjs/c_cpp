#include "stdafx.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "textureshaderclass.h"
#include "rendertextureclass.h"
#include "BitmapClass.h"
#include "FadeShaderClass.h"
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

	// 모델 객체 생성
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// 모델 객체 초기화
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), L"../RasterTek28/data/seafloor.dds", "../RasterTek28/data/cube.txt"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
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

	// 렌더링 텍스처 객체를 생성한다.
	m_RenderTexture = new RenderTextureClass;
	if(!m_RenderTexture)
	{
		return false;
	}

	// 렌더링 텍스처 객체를 초기화한다.
	if(!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
	{
		return false;
	}

	// 비트 맵 객체를 만듭니다.
	m_Bitmap = new BitmapClass;
	if(!m_Bitmap)
	{
		return false;
	}

	// 비트 맵 객체를 초기화합니다.
	if(!m_Bitmap->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight))
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	// 페이드 인 타임을 3000 밀리 초로 설정합니다.
	m_fadeInTime = 3000.0f;

	// 누적 된 시간을 0 밀리 초로 초기화합니다.
	m_accumulatedTime = 0;

	// 페이드 백분율을 처음에 0으로 초기화하여 장면이 검게 표시됩니다.
	m_fadePercentage = 0;

	// 효과가 사라지도록 설정합니다.
	m_fadeDone = false;

	// 페이드 셰이더 개체를 만듭니다.
	m_FadeShader = new FadeShaderClass;
	if(!m_FadeShader)
	{
		return false;
	}

	// 페이드 셰이더 개체를 초기화합니다.
	if(!m_FadeShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the fade shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// 페이드 셰이더 개체를 해제하십시오.
	if(m_FadeShader)
	{
		m_FadeShader->Shutdown();
		delete m_FadeShader;
		m_FadeShader = 0;
	}

	// 비트 맵 객체를 해제합니다.
	if(m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}

	// 렌더를 텍스쳐 객체로 릴리즈한다.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// 텍스처 쉐이더 객체를 해제한다.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 모델 객체 반환
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


bool GraphicsClass::Frame(float frameTime)
{
	if(!m_fadeDone)
	{
		// 누적 된 시간을 여분의 프레임 시간 추가로 업데이트하십시오.
		m_accumulatedTime += frameTime;

		// 시간이 갈수록 각 프레임을 통과하는 시간만큼 페이드 수가 증가합니다.
		if(m_accumulatedTime < m_fadeInTime)
		{
			// 누적 된 시간을 기준으로 화면이 희미해질 비율을 계산합니다.
			m_fadePercentage = m_accumulatedTime / m_fadeInTime;
		}
		else
		{
			// 페이드 인 타임이 완료되면 페이드 효과를 끄고 장면을 정상적으로 렌더링합니다.
			m_fadeDone = true;

			// 백분율을 100 %로 설정합니다.
			m_fadePercentage = 1.0f;
		}
	}

	// 카메라 위치를 설정합니다.
	m_Camera->SetPosition(0.0f, 0.0f, -6.0f);

	return true;
}


bool GraphicsClass::Render()
{
	bool result;
	static float rotation = 0.0f;


	// 각 프레임의 rotation 변수를 업데이트합니다.
	rotation += (float)XM_PI * 0.005f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	if(m_fadeDone)
	{
		// 페이드 인이 완료되면 백 버퍼를 사용하여 장면을 정상적으로 렌더링합니다.
		RenderNormalScene(rotation);
	}
	else
	{
		// 페이드 인이 완료되지 않은 경우 장면을 텍스처로 렌더링하고 텍스처를 페이드 인합니다.
		result = RenderToTexture(rotation);
		if(!result)
		{
			return false;
		}
	
		result = RenderFadingScene();
		if(!result)
		{
			return false;
		}
	}

	return true;
}

bool GraphicsClass::RenderToTexture(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	
	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// 렌더링을 텍스처에 지 웁니다.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 1.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();
	
	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 회전에 의해 월드 행렬에 곱합니다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더로 모델을 렌더링한다.
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix,
							projectionMatrix, m_Model->GetTexture());

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}


bool GraphicsClass::RenderFadingScene()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;


	// 장면을 시작할 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 비트 맵 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 0, 0);
	if(!result)
	{
		return false;
	}

	// 페이드 셰이더를 사용하여 비트 맵을 렌더링합니다.
	result = m_FadeShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
								  m_RenderTexture->GetShaderResourceView(), m_fadePercentage);
	if(!result)
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 된 장면을 화면에 표시합니다.
	m_Direct3D->EndScene();

	return true;
}


bool GraphicsClass::RenderNormalScene(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// 장면을 시작할 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 1.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 회전에 의해 월드 행렬에 곱합니다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더로 모델을 렌더링한다.
	if(!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model->GetTexture()))
	{
		return false;
	}

	// 렌더링 된 장면을 화면에 표시합니다.
	m_Direct3D->EndScene();

	return true;
}