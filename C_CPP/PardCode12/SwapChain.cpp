#include "SwapChain.h"
#include "RenderSystem.h"


SwapChain::SwapChain()
{
	std::cout << "SwapChain" << " Class" << " ������ ȣ��" << '\n';
}

SwapChain::~SwapChain()
{
	std::cout << "SwapChain" << " Class" << "�Ҹ��� ȣ��" << '\n';
	Release();
}

void SwapChain::Init(ID3D11Device* pDevice, HWND hwnd, UINT iWidth, UINT iHeight)
{
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
	desc_swapchain.BufferDesc.Width = iWidth;							//������� �ػ�
	desc_swapchain.BufferDesc.Height = iHeight;
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

	//����ۻ���� ���� �ڿ����ҽ���(����Ÿ�ٺ����)
	ID3D11Texture2D* backBuffer;
	hResult = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");

	hResult = pDevice->CreateRenderTargetView(backBuffer, NULL, &m_pRTV);
	_ASEERTION_CREATE(hResult, "RTV");

	//����� ������� ���۷��� ī������ �����
	backBuffer->Release();
}

//����Ÿ�� �ʱ�ȭ �� ��º��ձ⿡ ����Ÿ�ٺ� ����
void SwapChain::ClearRenderTargetColor(ID3D11DeviceContext* pDeviceContext, float red, float green, float blue, float alpha)
{
	FLOAT clearColor[] = { red, green, blue, alpha };
	pDeviceContext->ClearRenderTargetView(m_pRTV, clearColor);
	pDeviceContext->OMSetRenderTargets(1, &m_pRTV, NULL);
}

//������۸�
void SwapChain::Present(bool vsync)
{
	std::cout << "SwapchainPreset, ����۱�ü" << '\n';
	m_pSwapChain->Present(vsync, NULL);
}

void SwapChain::Release()
{
	m_pRTV->Release();
	m_pSwapChain->Release();
}
