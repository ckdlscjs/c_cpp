#include "GraphicEngine.h"
#include <iostream>

GraphicEngine::GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << " 생성자 호출" << '\n';
}

GraphicEngine::~GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << "소멸자 호출" << '\n';
}

bool GraphicEngine::Init()
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
	for (UINT driver_type_idx = 0; driver_type_idx < num_drivers; driver_type_idx++)
	{
		if (SUCCEEDED(D3D11CreateDevice
		(
			NULL,								//_In_opt_ IDXGIAdapter * pAdapter, 디스플레이 어댑터지정, 0(NULL)시 기본 디스플레이 어댑터 사용
			driver_types[driver_type_idx],		//D3D_DRIVER_TYPE DriverType, 렌더링에 어떠한 그래픽 가속 드라이버를 쓸것인가 GPU를 기본으로둔다
			NULL,								//HMODULE Software, 하드웨어를사용하므로null로둔다
			NULL,								//UINT Flags, 추가적 장치 생성 플래그들, 일반적으로 두개의 플래그를 선택해 사용한다(Debug플래그, 싱글스레드 성능 상향플래그)
			feature_levels,						//_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL * pFeatureLevels, 피쳐레벨 배열
			num_features,						//UINT FeatureLevels, 피쳐레벨의갯수
			D3D11_SDK_VERSION,					//UINT SDKVersion, sdk버전지정
			&m_D3DDevice,						//_COM_Outptr_opt_ ID3D11Device * *ppDevice, 반환할 ID3DDevice객체
			&m_FeatureLevel,					//_Out_opt_ D3D_FEATURE_LEVEL * pFeatureLevel, 반환할FeatureLevel
			&m_D3DDeviceContext					//_COM_Outptr_opt_ ID3D11DeviceContext * *ppImmediateContext, 반환할ID3DDeviceContext객체
		)))
			return true;
	}
	return false;
}

bool GraphicEngine::Release()
{
	m_D3DDevice->Release();
	m_D3DDevice = nullptr;

	m_D3DDeviceContext->Release();
	m_D3DDeviceContext = nullptr;
	return true;
}

//싱글톤객체, static선언으로 컴파일타임에 객체가 생성된다
GraphicEngine& GraphicEngine::Get()
{
	static GraphicEngine engine;
	return engine;
}
