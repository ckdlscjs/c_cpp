#include "SwapChain.h"
#include "GraphicEngine.h"


SwapChain::SwapChain()
{
	std::cout << "SwapChain" << " Class" << " ������ ȣ��" << '\n';
}

SwapChain::~SwapChain()
{
	std::cout << "SwapChain" << " Class" << "�Ҹ��� ȣ��" << '\n';
	Release();
}

bool SwapChain::Init(ID3D11Device* pDevice, HWND hwnd, UINT iWidth, UINT iHeight)
{
	//D3DDevice���� Factory���� �������� �����ؼ� Factory��ü���� �ּҸ� �޾ƿ´�
	//__uuidof�� �ش� �Լ����� �޾ƿ� ��ü�� ID�� �ǹ��ϸ� �ι�°���ڷ� (void**)�� ĳ������ �������� �������� �ּҰ��� �Ѱܾ� ���� m_������ ���������س������ִ�
	HRESULT hResult;
	IDXGIDevice* pDXGIDevice;
	IDXGIAdapter* pDXGIAdapter;
	IDXGIFactory* pDXGIFactory;
	hResult = pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	if (FAILED(hResult))
		return false;
	hResult = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);
	if (FAILED(hResult))
		return false;
	hResult = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);
	if (FAILED(hResult))
		return false;
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
	if (FAILED(hResult))
		return false;
	//������ DXGI��ü ���۷��� ī������ �����
	pDXGIDevice->Release();
	pDXGIAdapter->Release();
	pDXGIFactory->Release();

	//����ۻ���� ���� �ڿ����ҽ���(����Ÿ�ٺ����)
	ID3D11Texture2D* backBuffer;
	hResult = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hResult))
		return false;
	hResult = pDevice->CreateRenderTargetView(backBuffer, NULL, &m_pRTV);
	if (FAILED(hResult))
		return false;
	//����� ������� ���۷��� ī������ �����
	backBuffer->Release();
	return true;
}

bool SwapChain::ClearRenderTargetColor(ID3D11DeviceContext* pDeviceContext, float red, float green, float blue, float alpha)
{
	FLOAT clearColor[] = { red, green, blue, alpha };
	pDeviceContext->ClearRenderTargetView(m_pRTV, clearColor);
	pDeviceContext->OMSetRenderTargets(1, &m_pRTV, NULL);
	return false;
}

bool SwapChain::Present(bool vsync)
{
	std::cout << "SwapchainPreset, ����۱�ü" << '\n';
	m_pSwapChain->Present(vsync, NULL);
	return true;
}

ID3D11RenderTargetView* SwapChain::GetRenderTargetView() const
{
	return m_pRTV;
}

bool SwapChain::Release()
{
	m_pRTV->Release();
	m_pSwapChain->Release();
	return true;
}
