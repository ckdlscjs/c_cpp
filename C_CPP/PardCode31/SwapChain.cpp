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
	desc_swapchain.BufferCount = 1;																//������ǰ���, �Ϲ������δ� 1��
	desc_swapchain.BufferDesc.Width = width;													//������� �ػ�
	desc_swapchain.BufferDesc.Height = height;
	desc_swapchain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;								//�ȼ�����, rgba�� 8��Ʈ�� 32��Ʈ�ػ󵵸� �Ϲ������λ��
	desc_swapchain.BufferDesc.RefreshRate.Numerator = 60;										//60hz���ǹ�, �������÷���Ʈ
	desc_swapchain.BufferDesc.RefreshRate.Denominator = 1;
	desc_swapchain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;		//�����ǻ��ó, ����Ÿ����¿�, SRV���ε��߰�
	desc_swapchain.OutputWindow = hwnd;															//�������� ��� �������ڵ�
	desc_swapchain.SampleDesc.Count = 1;														//��Ƽ���ø� ����, 1�̸� ������ 4�̻��̸� MSAA����
	desc_swapchain.SampleDesc.Quality = 0;														//���ø�ǰ��, 0�̸� ���밪
	desc_swapchain.Windowed = TRUE;																//TRUE:â��� FALSE:��üȭ��

	hResult = pDXGIFactory->CreateSwapChain(pDevice, &desc_swapchain, &m_pSwapChain);
	_ASEERTION_CREATE(hResult, "SwapChain");

	//������ DXGI��ü ���۷��� ī������ �����
	pDXGIDevice->Release();
	pDXGIAdapter->Release();
	pDXGIFactory->Release();
}

SwapChain::~SwapChain()
{
	std::cout << "Release : " << "SwapChain" << " Class" << '\n';
	if (m_pSwapChain)
		m_pSwapChain->Release();
}
IDXGISwapChain* SwapChain::GetSwapChain()
{
	return m_pSwapChain;
}


