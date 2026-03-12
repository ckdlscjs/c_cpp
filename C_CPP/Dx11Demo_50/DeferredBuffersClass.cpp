#include "stdafx.h"
#include "deferredbuffersclass.h"


DeferredBuffersClass::DeferredBuffersClass()
{
	for(int i=0; i<BUFFER_COUNT; i++)
	{
		m_renderTargetTextureArray[i] = 0;
		m_renderTargetViewArray[i] = 0;
		m_shaderResourceViewArray[i] = 0;
	}

	m_depthStencilBuffer = 0;
	m_depthStencilView = 0;
}


DeferredBuffersClass::DeferredBuffersClass(const DeferredBuffersClass& other)
{
}


DeferredBuffersClass::~DeferredBuffersClass()
{
}


bool DeferredBuffersClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{
	
	HRESULT result = S_OK;

	// 렌더 텍스처의 폭과 높이를 저장한다.
	m_textureWidth = textureWidth;
	m_textureHeight = textureHeight;

	// 렌더 타겟 텍스처 구조체를 초기화합니다.
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// 렌더 타겟 텍스처 구조체를 설정합니다.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

	// 렌더 타겟 텍스처를 만듭니다.
	for(int i=0; i<BUFFER_COUNT; i++)
	{
		result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTextureArray[i]);
		if(FAILED(result))
		{
			return false;
		}
	}

	// 렌더 타겟 뷰의 구조체를 설정합니다.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// 렌더 타겟 뷰를 생성합니다.
	for(int i=0; i<BUFFER_COUNT; i++)
	{
		result = device->CreateRenderTargetView(m_renderTargetTextureArray[i], &renderTargetViewDesc, &m_renderTargetViewArray[i]);
		if(FAILED(result))
		{
			return false;
		}
	}

	// 셰이더 리소스 뷰의 구조체를 설정합니다.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// 셰이더 리소스 뷰를 만듭니다.
	for(int i=0; i<BUFFER_COUNT; i++)
	{
		result = device->CreateShaderResourceView(m_renderTargetTextureArray[i], &shaderResourceViewDesc, &m_shaderResourceViewArray[i]);
		if(FAILED(result))
		{
			return false;
		}
	}

	// 깊이 버퍼의 구조체를 초기화합니다.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이 버퍼의 구조체를 설정합니다.
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 채워진 구조체를 사용하여 깊이 버퍼의 텍스처를 만듭니다.
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// 깊이 스텐실 뷰 구조체를 초기화합니다.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이 스텐실 뷰 구조체를 설정합니다.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이 스텐실 뷰를 만듭니다.
	result = device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result))
	{
		return false;
	}

	// 렌더링을 위해 뷰포트를 설정합니다.
    m_viewport.Width = (float)textureWidth;
    m_viewport.Height = (float)textureHeight;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;

	return true;
}


void DeferredBuffersClass::Shutdown()
{
	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	for(int i=0; i<BUFFER_COUNT; i++)
	{
		if(m_shaderResourceViewArray[i])
		{
			m_shaderResourceViewArray[i]->Release();
			m_shaderResourceViewArray[i] = 0;
		}

		if(m_renderTargetViewArray[i])
		{
			m_renderTargetViewArray[i]->Release();
			m_renderTargetViewArray[i] = 0;
		}

		if(m_renderTargetTextureArray[i])
		{
			m_renderTargetTextureArray[i]->Release();
			m_renderTargetTextureArray[i] = 0;
		}
	}
}


void DeferredBuffersClass::SetRenderTargets(ID3D11DeviceContext* deviceContext)
{
	// 렌더링 대상 뷰 배열 및 깊이 스텐실 버퍼를 출력 렌더 파이프 라인에 바인딩 합니다.
	deviceContext->OMSetRenderTargets(BUFFER_COUNT, m_renderTargetViewArray, m_depthStencilView);
	
	// 뷰포트를 설정합니다.
	deviceContext->RSSetViewports(1, &m_viewport);
}


void DeferredBuffersClass::ClearRenderTargets(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha)
{
	// 버퍼를 지울 색을 설정합니다.
	float color[4] = { red, green, blue, alpha };

	// 렌더 타겟 버퍼를 지웁니다.
	for(int i=0; i<BUFFER_COUNT; i++)
	{
		deviceContext->ClearRenderTargetView(m_renderTargetViewArray[i], color);
	}

	// 깊이 버퍼를 지웁니다.
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


ID3D11ShaderResourceView* DeferredBuffersClass::GetShaderResourceView(int view)
{
	return m_shaderResourceViewArray[view];
}