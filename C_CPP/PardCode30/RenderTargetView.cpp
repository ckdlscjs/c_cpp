#include "RenderTargetView.h"

RenderTargetView::RenderTargetView(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain)
{
	std::cout << "Initialize : " << "RenderTargetView" << " Class" << '\n';
	Resize(pDevice, pSwapChain);
}

RenderTargetView::RenderTargetView(ID3D11Device* pDevice, UINT width, UINT height)
{
	std::cout << "Initialize : " << "RenderTargetView" << " Class" << '\n';
	Resize(pDevice, width, height);
}

RenderTargetView::~RenderTargetView()
{
	std::cout << "Release : " << "RenderTargetView" << " Class" << '\n';
	if (m_pRTV)
		m_pRTV->Release();
}

void RenderTargetView::Resize(ID3D11Device* pDevice, IDXGISwapChain* pSwapChain)
{
	HRESULT hResult;
	ID3D11Texture2D* pBuffer;
	hResult = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");

	//넘겨받은 백버퍼를 기반으로 RendrTargetView를 생성
	hResult = pDevice->CreateRenderTargetView(pBuffer, NULL, &m_pRTV);
	_ASEERTION_CREATE(hResult, "RTV");

	//백버퍼는 SRV를 생성하지않는다
	/*
	//RTV와 같은 백버퍼를 기반으로 하는 SRV를 생성
	D3D11_RENDER_TARGET_VIEW_DESC desc_rtv;
	m_pRTV->GetDesc(&desc_rtv);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
	ZeroMemory(&desc_srv, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	desc_srv.Format = desc_rtv.Format;
	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc_srv.Texture2D.MostDetailedMip = 0;
	desc_srv.Texture2D.MipLevels = 1;
	hResult = pDevice->CreateShaderResourceView(pBuffer, &desc_srv, &m_pSRV);
	_ASEERTION_CREATE(hResult, "SRV");
	*/
	pBuffer->Release();
}

void RenderTargetView::Resize(ID3D11Device* pDevice, UINT width, UINT height)
{
	ID3D11Texture2D* pBuffer;
	HRESULT hResult;

	// 렌더 타겟 텍스처 설명을 초기화합니다.
	D3D11_TEXTURE2D_DESC desc_tex;
	ZeroMemory(&desc_tex, sizeof(D3D11_TEXTURE2D_DESC));

	// 렌더 타겟 텍스처 설명을 설정합니다.
	desc_tex.Width = width;
	desc_tex.Height = height;
	desc_tex.MipLevels = 1;
	desc_tex.ArraySize = 1;
	desc_tex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc_tex.SampleDesc.Count = 1;
	desc_tex.Usage = D3D11_USAGE_DEFAULT;
	desc_tex.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc_tex.CPUAccessFlags = 0;
	desc_tex.MiscFlags = 0;

	// 렌더 타겟 텍스처를 만듭니다.
	hResult = pDevice->CreateTexture2D(&desc_tex, NULL, &pBuffer);
	_ASEERTION_CREATE(hResult, "Buffer");

	// 렌더 타겟 뷰의 설명을 설정합니다.
	D3D11_RENDER_TARGET_VIEW_DESC desc_rtv;
	desc_rtv.Format = desc_tex.Format;
	desc_rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	desc_rtv.Texture2D.MipSlice = 0;

	// 렌더 타겟 뷰를 생성한다.
	hResult = pDevice->CreateRenderTargetView(pBuffer, &desc_rtv, &m_pRTV);
	_ASEERTION_CREATE(hResult, "RTV");

	// 셰이더 리소스 뷰의 설명을 설정합니다.
	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
	desc_srv.Format = desc_tex.Format;
	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc_srv.Texture2D.MostDetailedMip = 0;
	desc_srv.Texture2D.MipLevels = 1;

	// 셰이더 리소스 뷰를 만듭니다.
	hResult = pDevice->CreateShaderResourceView(pBuffer, &desc_srv, &m_pSRV);
	_ASEERTION_CREATE(hResult, "SRV");

	pBuffer->Release();
}

ID3D11RenderTargetView* RenderTargetView::GetRTV()
{
	return m_pRTV;
}
