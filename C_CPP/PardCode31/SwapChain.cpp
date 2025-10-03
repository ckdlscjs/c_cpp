#include "SwapChain.h"

SwapChain::SwapChain(ID3D11Device* pDevice, HWND hwnd, UINT width, UINT height)
{
	std::cout << "Initialize : " << "SwapChain" << " Class" << '\n';
	//D3DDevice부터 Factory까지 역순으로 참조해서 Factory객체까지 주소를 받아온다
	//__uuidof는 해당 함수에서 받아올 객체의 ID를 의미하며 두번째인자로 (void**)로 캐스팅후 저장해줄 포인터의 주소값을 넘겨야 실제 m_변수에 값을저장해놓을수있다
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
	desc_swapchain.BufferCount = 1;																//백버퍼의갯수, 일반적으로는 1개
	desc_swapchain.BufferDesc.Width = width;													//백버퍼의 해상도
	desc_swapchain.BufferDesc.Height = height;
	desc_swapchain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;								//픽셀포맷, rgba각 8비트씩 32비트해상도를 일반적으로사용
	desc_swapchain.BufferDesc.RefreshRate.Numerator = 60;										//60hz를의미, 리프레시레이트
	desc_swapchain.BufferDesc.RefreshRate.Denominator = 1;
	desc_swapchain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;		//버퍼의사용처, 렌더타겟출력용, SRV바인딩추가
	desc_swapchain.OutputWindow = hwnd;															//렌더링할 대상 윈도우핸들
	desc_swapchain.SampleDesc.Count = 1;														//멀티샘플링 설정, 1이면 사용안함 4이상이면 MSAA적용
	desc_swapchain.SampleDesc.Quality = 0;														//샘플링품질, 0이면 보통값
	desc_swapchain.Windowed = TRUE;																//TRUE:창모드 FALSE:전체화면

	hResult = pDXGIFactory->CreateSwapChain(pDevice, &desc_swapchain, &m_pSwapChain);
	_ASEERTION_CREATE(hResult, "SwapChain");

	//사용다한 DXGI객체 레퍼런스 카운팅을 낮춘다
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


