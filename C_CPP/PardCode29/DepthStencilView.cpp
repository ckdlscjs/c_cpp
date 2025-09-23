#include "DepthStencilView.h"

DepthStencilView::DepthStencilView(ID3D11Device* pDevice, UINT width, UINT height)
{
	std::cout << "Initialize : " << "DepthStencilView" << " Class" << '\n';
	Resize(pDevice, width, height);
}

DepthStencilView::~DepthStencilView()
{
	std::cout << "Release : " << "DepthStencilView" << " Class" << '\n';
	if (m_pDSV)m_pDSV->Release();
}


ID3D11DepthStencilView* DepthStencilView::GetDSV()
{
	return m_pDSV;
}

void DepthStencilView::Resize(ID3D11Device* pDevice, UINT width, UINT height)
{
	HRESULT hResult;
	ID3D11Texture2D* pBuffer;

	//뎁스스텐실뷰에 사용할 텍스쳐버퍼 디스크립션
	D3D11_TEXTURE2D_DESC tex_desc;
	ZeroMemory(&tex_desc, sizeof(D3D11_TEXTURE2D_DESC));
	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;						//dsv, srv에 이용하기위해 TYPELESS로이용한다
	tex_desc.SampleDesc.Count = 1;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0;										// AccessFlag 값의 조합
	tex_desc.MiscFlags = 0;												// 텍스처의 다양한 속성(큐브맵, 배열 등을 제어하는 기타플래그)

	hResult = pDevice->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
	_ASEERTION_CREATE(hResult, "DepthBuffer");

	D3D11_DEPTH_STENCIL_VIEW_DESC desc_dsv;
	ZeroMemory(&desc_dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc_dsv.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc_dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc_dsv.Texture2D.MipSlice = 0;
	hResult = pDevice->CreateDepthStencilView(pBuffer, &desc_dsv, &m_pDSV);	//해당버퍼를 이용하여 깊이스텐실 뷰를 생성	
	_ASEERTION_CREATE(hResult, "DSV");

	D3D11_SHADER_RESOURCE_VIEW_DESC desc_srv;
	desc_srv.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	desc_srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc_srv.Texture2D.MostDetailedMip = 0;
	desc_srv.Texture2D.MipLevels = 1;
	hResult = pDevice->CreateShaderResourceView(pBuffer, &desc_srv, &m_pSRV);
	_ASEERTION_CREATE(hResult, "SRV");

	pBuffer->Release();
}
