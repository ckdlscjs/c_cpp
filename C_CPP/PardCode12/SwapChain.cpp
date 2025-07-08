#include "SwapChain.h"
#include "RenderSystem.h"


SwapChain::SwapChain()
{
	std::cout << "SwapChain" << " Class" << " 생성자 호출" << '\n';
}

SwapChain::~SwapChain()
{
	std::cout << "SwapChain" << " Class" << "소멸자 호출" << '\n';
	Release();
}

void SwapChain::Init(ID3D11Device* pDevice, HWND hwnd, UINT iWidth, UINT iHeight)
{
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

	hResult = pDXGIFactory->CreateSwapChain(pDevice, &desc_swapchain, &m_pSwapChain);
	_ASEERTION_CREATE(hResult, "SwapChain");

	//사용다한 DXGI객체 레퍼런스 카운팅을 낮춘다
	pDXGIDevice->Release();
	pDXGIAdapter->Release();
	pDXGIFactory->Release();

	//백버퍼사용을 위한 자원리소스뷰(렌더타겟뷰생성)
	ID3D11Texture2D* backBuffer;
	hResult = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");

	hResult = pDevice->CreateRenderTargetView(backBuffer, NULL, &m_pRTV);
	_ASEERTION_CREATE(hResult, "RTV");

	//사용한 백버퍼의 레퍼런스 카운팅을 낮춘다
	backBuffer->Release();
}

//렌더타겟 초기화 및 출력병합기에 렌더타겟뷰 세팅
void SwapChain::ClearRenderTargetColor(ID3D11DeviceContext* pDeviceContext, float red, float green, float blue, float alpha)
{
	FLOAT clearColor[] = { red, green, blue, alpha };
	pDeviceContext->ClearRenderTargetView(m_pRTV, clearColor);
	pDeviceContext->OMSetRenderTargets(1, &m_pRTV, NULL);
}

//더블버퍼링
void SwapChain::Present(bool vsync)
{
	std::cout << "SwapchainPreset, 백버퍼교체" << '\n';
	m_pSwapChain->Present(vsync, NULL);
}

void SwapChain::Release()
{
	m_pRTV->Release();
	m_pSwapChain->Release();
}
