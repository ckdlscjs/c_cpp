#include "DepthStencilView.h"

DepthStencilView::DepthStencilView(ID3D11Device* pDevice, UINT width, UINT height)
{
	HRESULT hResult;
	//임시, 뎁스스텐실사용 추후 클래스화필요(10장, 깊이스텐실부분)
	ID3D11Texture2D* pBuffer;
	D3D11_TEXTURE2D_DESC tex_desc;
	ZeroMemory(&tex_desc, sizeof(D3D11_TEXTURE2D_DESC));
	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex_desc.CPUAccessFlags = 0;										// AccessFlag 값의 조합
	tex_desc.MiscFlags = 0;												// 텍스처의 다양한 속성(큐브맵, 배열 등을 제어하는 기타플래그)

	hResult = pDevice->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
	_ASEERTION_CREATE(hResult, "DepthBuffer");
	hResult = pDevice->CreateDepthStencilView(pBuffer, NULL, &m_pDSV);	//해당버퍼를 이용하여 깊이스텐실 뷰를 생성															//임의의 사용한 버퍼를 제거
	_ASEERTION_CREATE(hResult, "DSV");
	pBuffer->Release();
}

DepthStencilView::~DepthStencilView()
{
}
