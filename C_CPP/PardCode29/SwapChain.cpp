#include "SwapChain.h"

SwapChain::SwapChain(ID3D11Device* pDevice, HWND hwnd, UINT width, UINT height)
{
	std::cout << "Initialize : " << "SwapChain" << " Class" << '\n';
	//D3DDevice���� Factory���� �������� �����ؼ� Factory��ü���� �ּҸ� �޾ƿ´�
	//__uuidof�� �ش� �Լ����� �޾ƿ� ��ü�� ID�� �ǹ��ϸ� �ι�°���ڷ� (void**)�� ĳ������ �������� �������� �ּҰ��� �Ѱܾ� ���� m_������ ���������س������ִ�
	HRESULT hResult;
	IDXGIDevice* pDXGIDevice;
	IDXGIAdapter* pDXGIAdapter;
	IDXGIFactory* pDXGIFactory;
	hResult = pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	_ASEERTION_CREATE(hResult, "DXGIDevice");

	hResult = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);
	_ASEERTION_CREATE(hResult, "DXGIAdapter");

	hResult = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);
	_ASEERTION_CREATE(hResult, "DXGIFactory");

	DXGI_SWAP_CHAIN_DESC desc_swapchain;
	ZeroMemory(&desc_swapchain, sizeof(desc_swapchain));
	desc_swapchain.BufferCount = 1;										//������ǰ���, �Ϲ������δ� 1��
	desc_swapchain.BufferDesc.Width = width;							//������� �ػ�
	desc_swapchain.BufferDesc.Height = height;
	desc_swapchain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		//�ȼ�����, rgba�� 8��Ʈ�� 32��Ʈ�ػ󵵸� �Ϲ������λ��
	desc_swapchain.BufferDesc.RefreshRate.Numerator = 60;				//60hz���ǹ�, �������÷���Ʈ
	desc_swapchain.BufferDesc.RefreshRate.Denominator = 1;
	desc_swapchain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		//�����ǻ��ó, ����Ÿ����¿�
	desc_swapchain.OutputWindow = hwnd;									//�������� ��� �������ڵ�
	desc_swapchain.SampleDesc.Count = 1;								//��Ƽ���ø� ����, 1�̸� ������ 4�̻��̸� MSAA����
	desc_swapchain.SampleDesc.Quality = 0;								//���ø�ǰ��, 0�̸� ���밪
	desc_swapchain.Windowed = TRUE;										//TRUE:â��� FALSE:��üȭ��

	hResult = pDXGIFactory->CreateSwapChain(pDevice, &desc_swapchain, &m_pSwapChain);
	_ASEERTION_CREATE(hResult, "SwapChain");

	//������ DXGI��ü ���۷��� ī������ �����
	pDXGIDevice->Release();
	pDXGIAdapter->Release();
	pDXGIFactory->Release();

	CreateRenderTargetView(pDevice);
	CreateDepthStencilView(pDevice, width, height);
}

SwapChain::~SwapChain()
{
	std::cout << "Release : " << "SwapChain" << " Class" << '\n';
	if(m_pRenderTargetView)
		m_pRenderTargetView->Release();
	if(m_pDepthStencilView)
		m_pDepthStencilView->Release();
	if(m_pSwapChain)
		m_pSwapChain->Release();
}

void SwapChain::CreateRenderTargetView(ID3D11Device* pDevice)
{
	HRESULT hResult;
	ID3D11Texture2D* backBuffer;	//����ۻ���� ���� �ڿ����ҽ���(����Ÿ�ٺ����)

	hResult = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");

	hResult = pDevice->CreateRenderTargetView(backBuffer, NULL, &m_pRenderTargetView);
	_ASEERTION_CREATE(hResult, "RTV");

	//����� ������� ���۷��� ī������ �����
	backBuffer->Release();
}

void SwapChain::CreateDepthStencilView(ID3D11Device* pDevice, UINT width, UINT height)
{
	HRESULT hResult;
	ID3D11Texture2D* backBuffer; //����ۻ���� ���� �ڿ����ҽ���(�������ٽǺ� ����)	

	//�ӽ�, �������ٽǻ�� ���� Ŭ����ȭ�ʿ�(10��, ���̽��ٽǺκ�)
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
	tex_desc.CPUAccessFlags = 0;					// AccessFlag ���� ����
	tex_desc.MiscFlags = 0;							// �ؽ�ó�� �پ��� �Ӽ�(ť���, �迭 ���� �����ϴ� ��Ÿ�÷���)

	hResult = pDevice->CreateTexture2D(&tex_desc, nullptr, &backBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");
	hResult = pDevice->CreateDepthStencilView(backBuffer, NULL, &m_pDepthStencilView);	//�ش���۸� �̿��Ͽ� ���̽��ٽ� �並 ����
	backBuffer->Release();																//������ ����� ���۸� ����
	_ASEERTION_CREATE(hResult, "DSV");
}

//����۸� �缳���Ѵ�, flags�� �ʿ信���� ����, �⺻ = 0
void SwapChain::ResizeBuffers(UINT bufferCount, UINT width, UINT height, DXGI_FORMAT format, UINT flags)
{
	//������ ����ϴ� ����Ÿ�ٺ䰡 ������ �ʱ�ȭ
	if (m_pRenderTargetView)
		m_pRenderTargetView->Release();
	//������ ����ϴ� ���� ���ٽǺ䰡 ������ �ʱ�ȭ
	if (m_pDepthStencilView)
		m_pDepthStencilView->Release();
	m_pSwapChain->ResizeBuffers(bufferCount, width, height, format, flags);
}

//����Ÿ�� �ʱ�ȭ
void SwapChain::ClearRenderTargetColor(ID3D11DeviceContext* pDeviceContext, float red, float green, float blue, float alpha)
{
	FLOAT clearColor[] = { red, green, blue, alpha };
	pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
	pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
}

//������۸�
void SwapChain::Present(bool vsync)
{
	std::cout << "SwapchainPreset, ����۱�ü" << '\n';
	m_pSwapChain->Present(vsync, NULL);
}

