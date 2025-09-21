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
	desc_swapchain.BufferCount = 1;										//백버퍼의갯수, 일반적으로는 1개
	desc_swapchain.BufferDesc.Width = width;							//백버퍼의 해상도
	desc_swapchain.BufferDesc.Height = height;
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
	ID3D11Texture2D* backBuffer;	//백버퍼사용을 위한 자원리소스뷰(렌더타겟뷰생성)

	hResult = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");

	hResult = pDevice->CreateRenderTargetView(backBuffer, NULL, &m_pRenderTargetView);
	_ASEERTION_CREATE(hResult, "RTV");

	//사용한 백버퍼의 레퍼런스 카운팅을 낮춘다
	backBuffer->Release();
}

void SwapChain::CreateDepthStencilView(ID3D11Device* pDevice, UINT width, UINT height)
{
	HRESULT hResult;
	ID3D11Texture2D* backBuffer; //백버퍼사용을 위한 자원리소스뷰(뎁스스텐실뷰 생성)	

	//임시, 뎁스스텐실사용 추후 클래스화필요(10장, 깊이스텐실부분)
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
	tex_desc.CPUAccessFlags = 0;					// AccessFlag 값의 조합
	tex_desc.MiscFlags = 0;							// 텍스처의 다양한 속성(큐브맵, 배열 등을 제어하는 기타플래그)

	hResult = pDevice->CreateTexture2D(&tex_desc, nullptr, &backBuffer);
	_ASEERTION_CREATE(hResult, "BackBuffer");
	hResult = pDevice->CreateDepthStencilView(backBuffer, NULL, &m_pDepthStencilView);	//해당버퍼를 이용하여 깊이스텐실 뷰를 생성
	backBuffer->Release();																//임의의 사용한 버퍼를 제거
	_ASEERTION_CREATE(hResult, "DSV");
}

//백버퍼를 재설정한다, flags는 필요에따라 설정, 기본 = 0
void SwapChain::ResizeBuffers(UINT bufferCount, UINT width, UINT height, DXGI_FORMAT format, UINT flags)
{
	//기존에 사용하던 렌더타겟뷰가 있을때 초기화
	if (m_pRenderTargetView)
		m_pRenderTargetView->Release();
	//기존에 사용하던 뎁스 스텐실뷰가 있을때 초기화
	if (m_pDepthStencilView)
		m_pDepthStencilView->Release();
	m_pSwapChain->ResizeBuffers(bufferCount, width, height, format, flags);
}

//렌더타겟 초기화
void SwapChain::ClearRenderTargetColor(ID3D11DeviceContext* pDeviceContext, float red, float green, float blue, float alpha)
{
	FLOAT clearColor[] = { red, green, blue, alpha };
	pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
	pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
}

//더블버퍼링
void SwapChain::Present(bool vsync)
{
	std::cout << "SwapchainPreset, 백버퍼교체" << '\n';
	m_pSwapChain->Present(vsync, NULL);
}

