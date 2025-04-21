#include "GraphicEngine.h"
#include "SwapChain.h"
#include "Window.h"

GraphicEngine::GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << " 생성자 호출" << '\n';
}

GraphicEngine::~GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << "소멸자 호출" << '\n';
}

bool GraphicEngine::Init(Window* pWindow)
{
	//드라이버타입
	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,			//하드웨어기반, 성능이제일좋음
		D3D_DRIVER_TYPE_WARP,				//중간
		D3D_DRIVER_TYPE_SOFTWARE,			//소프트웨어기반, 성능이제일안좋음
	};
	UINT num_drivers = ARRAYSIZE(driver_types); //sizeof

	//피쳐레벨
	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};
	UINT num_features = ARRAYSIZE(feature_levels);

	//CreateDevice, DeviceContext
	HRESULT hResult = 0;
	for (UINT driver_type_idx = 0; driver_type_idx < num_drivers; driver_type_idx++)
	{
		hResult = D3D11CreateDevice
		(
			NULL,								//_In_opt_ IDXGIAdapter * pAdapter, 디스플레이 어댑터지정, 0(NULL)시 기본 디스플레이 어댑터 사용
			driver_types[driver_type_idx],		//D3D_DRIVER_TYPE DriverType, 렌더링에 어떠한 그래픽 가속 드라이버를 쓸것인가 GPU를 기본으로둔다
			NULL,								//HMODULE Software, 하드웨어를사용하므로null로둔다
			NULL,								//UINT Flags, 추가적 장치 생성 플래그들, 일반적으로 두개의 플래그를 선택해 사용한다(Debug플래그, 싱글스레드 성능 상향플래그)
			feature_levels,						//_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL * pFeatureLevels, 피쳐레벨 배열
			num_features,						//UINT FeatureLevels, 피쳐레벨의갯수
			D3D11_SDK_VERSION,					//UINT SDKVersion, sdk버전지정
			&m_pD3D_Device,						//_COM_Outptr_opt_ ID3D11Device * *ppDevice, 반환할 ID3DDevice객체
			&m_D3D_FeatureLevel,					//_Out_opt_ D3D_FEATURE_LEVEL * pFeatureLevel, 반환할FeatureLevel
			&m_pD3D_DeviceContext					//_COM_Outptr_opt_ ID3D11DeviceContext * *ppImmediateContext, 반환할ID3DDeviceContext객체
		);
		if (SUCCEEDED(hResult))
			break;
	}
	if (FAILED(hResult))
		return false;
	//D3DDevice부터 Factory까지 역순으로 참조해서 Factory객체까지 주소를 받아온다
	//__uuidof는 해당 함수에서 받아올 객체의 ID를 의미하며 두번째인자로 (void**)로 캐스팅후 저장해줄 포인터의 주소값을 넘겨야 실제 m_변수에 값을저장해놓을수있다
	hResult = m_pD3D_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_pDXGI_Device);
	if (FAILED(hResult)) 
		return false;
	hResult = m_pDXGI_Device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_pDXGI_Adapter);
	if (FAILED(hResult))
		return false;
	hResult = m_pDXGI_Adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_pDXGI_Factory);
	if (FAILED(hResult))
		return false;
		
	m_pCSwapChain = new SwapChain();
	RECT rect = pWindow->GetClientWindowRect();
	m_pCSwapChain->Init(m_pDXGI_Factory, m_pD3D_Device, pWindow->GetHwnd(), rect.right - rect.left, rect.bottom - rect.top);
	return true;
}

bool GraphicEngine::Release()
{
	delete m_pCSwapChain;
	m_pCSwapChain = nullptr;

	m_pDXGI_Factory->Release();
	m_pDXGI_Adapter->Release();
	m_pDXGI_Device->Release();

	m_pD3D_Device->Release();
	m_pD3D_DeviceContext->Release();
	return true;
}

//싱글톤객체, static선언으로 컴파일타임에 객체가 생성된다
GraphicEngine& GraphicEngine::Get()
{
	static GraphicEngine engine;
	return engine;
}
