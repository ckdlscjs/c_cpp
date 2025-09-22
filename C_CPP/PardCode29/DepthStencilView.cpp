#include "DepthStencilView.h"

DepthStencilView::DepthStencilView(ID3D11Device* pDevice, UINT width, UINT height)
{
	HRESULT hResult;
	//�ӽ�, �������ٽǻ�� ���� Ŭ����ȭ�ʿ�(10��, ���̽��ٽǺκ�)
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
	tex_desc.CPUAccessFlags = 0;										// AccessFlag ���� ����
	tex_desc.MiscFlags = 0;												// �ؽ�ó�� �پ��� �Ӽ�(ť���, �迭 ���� �����ϴ� ��Ÿ�÷���)

	hResult = pDevice->CreateTexture2D(&tex_desc, nullptr, &pBuffer);
	_ASEERTION_CREATE(hResult, "DepthBuffer");
	hResult = pDevice->CreateDepthStencilView(pBuffer, NULL, &m_pDSV);	//�ش���۸� �̿��Ͽ� ���̽��ٽ� �並 ����															//������ ����� ���۸� ����
	_ASEERTION_CREATE(hResult, "DSV");
	pBuffer->Release();
}

DepthStencilView::~DepthStencilView()
{
}
