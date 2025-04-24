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

bool SwapChain::Init(IDXGIFactory* pFactory, ID3D11Device* pDevice, HWND hwnd, UINT iWidth, UINT iHeight)
{
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
	HRESULT hResult = pFactory->CreateSwapChain(pDevice, &desc_swapchain, &m_pSwapChain);
	if (FAILED(hResult))
		return false;

	//����ۻ���� ���� �ڿ����ҽ���(����Ÿ�ٺ����)
	ID3D11Texture2D* backBuffer;
	hResult = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hResult))
		return false;
	hResult = pDevice->CreateRenderTargetView(backBuffer, NULL, &m_pRTV);
	if (FAILED(hResult))
		return false;
	//����� ����۸� �������ش�
	backBuffer->Release();
	return true;
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
