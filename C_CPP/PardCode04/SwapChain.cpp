#include "SwapChain.h"
#include "GraphicEngine.h"


SwapChain::SwapChain()
{
	std::cout << "SwapChain" << " Class" << " 생성자 호출" << '\n';
}

SwapChain::~SwapChain()
{
	std::cout << "SwapChain" << " Class" << "소멸자 호출" << '\n';
	Release();
}

bool SwapChain::Init(IDXGIFactory* pFactory, ID3D11Device* pDevice, HWND hwnd, UINT iWidth, UINT iHeight)
{
	DXGI_SWAP_CHAIN_DESC desc_swapchain;
	ZeroMemory(&desc_swapchain, sizeof(desc_swapchain));
	desc_swapchain.BufferCount = 1;										//백버퍼의갯수, 일반적으로는 1개
	desc_swapchain.BufferDesc.Width = iWidth;							//백버퍼의 해상도
	desc_swapchain.BufferDesc.Height = iHeight;
	desc_swapchain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		//픽셀포맷, rgba각 8비트씩 32비트해상도를 일반적으로사용
	desc_swapchain.BufferDesc.RefreshRate.Numerator = 60;				//60hz를의미, 리프레시레이트
	desc_swapchain.BufferDesc.RefreshRate.Denominator = 1;				
	desc_swapchain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		//버퍼의사용처, 렌더타겟출력용
	desc_swapchain.OutputWindow = hwnd;									//렌더링할 대상 윈도우핸들
	desc_swapchain.SampleDesc.Count = 1;								//멀티샘플링 설정, 1이면 사용안함 4이상이면 MSAA적용
	desc_swapchain.SampleDesc.Quality = 0;								//샘플링품질, 0이면 보통값
	desc_swapchain.Windowed = TRUE;										//TRUE:창모드 FALSE:전체화면
	HRESULT hResult = pFactory->CreateSwapChain(pDevice, &desc_swapchain, &m_pSwapChain);
	if (FAILED(hResult))
		return false;

	//백버퍼사용을 위한 자원리소스뷰(렌더타겟뷰생성)
	ID3D11Texture2D* backBuffer;
	hResult = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hResult))
		return false;
	hResult = pDevice->CreateRenderTargetView(backBuffer, NULL, &m_pRTV);
	if (FAILED(hResult))
		return false;
	//사용한 백버퍼를 해제해준다
	backBuffer->Release();
	return true;
}

bool SwapChain::Present(bool vsync)
{
	std::cout << "SwapchainPreset, 백버퍼교체" << '\n';
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
