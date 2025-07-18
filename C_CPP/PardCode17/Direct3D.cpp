#include "Direct3D.h"
#include "SwapChain.h"
Direct3D::Direct3D()
{
	std::cout << "Direct3D" << " Class" << " 생성자 호출" << '\n';
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
			&m_D3D_FeatureLevel,				//_Out_opt_ D3D_FEATURE_LEVEL * pFeatureLevel, 반환할FeatureLevel
			&m_pD3D_DeviceContext				//_COM_Outptr_opt_ ID3D11DeviceContext * *ppImmediateContext, 반환할ID3DDeviceContext객체
		);
		if (SUCCEEDED(hResult))
			break;
	}
	_ASEERTION_CREATE(hResult, "Device&DeviceContext");
}

Direct3D::~Direct3D()
{
	std::cout << "Direct3D" << " Class" << "소멸자 호출" << '\n';
	m_pD3D_Device->Release();
	m_pD3D_DeviceContext->Release();
}

void Direct3D::SetViewportSize(UINT iWidth, UINT iHeight)
{
	D3D11_VIEWPORT vp = {};
	vp.Width = (float)iWidth;
	vp.Height = (float)iHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_pD3D_DeviceContext->RSSetViewports(1, &vp);
}

void Direct3D::DrawVertex_TriangleList(UINT vertexCount, UINT startIdx)
{
	m_pD3D_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//Vertex를 TrangleList로그린다
	m_pD3D_DeviceContext->Draw(vertexCount, startIdx);										//입력받은 vertex_count, 시작번호
}

void Direct3D::DrawVertex_TriangleStrip(UINT vertexCount, UINT startIdx)
{
	m_pD3D_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	//Vertex를 TrangleStrip
	m_pD3D_DeviceContext->Draw(vertexCount, startIdx);
}

void Direct3D::DrawIndex_TriagleList(UINT indexCount, UINT startIdx, INT vertexOffset)
{
	m_pD3D_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//Index를 TrangleList로그린다
	m_pD3D_DeviceContext->DrawIndexed(indexCount, startIdx, vertexOffset);					//시작인덱스, 인덱스버퍼의 시작번호, 버텍스버퍼의 오프셋값
}

void Direct3D::DrawIndex_TriangleStrip(UINT indexCount, UINT startIdx, INT vertexOffset)
{
	m_pD3D_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	//Index를 TrangleStrip
	m_pD3D_DeviceContext->DrawIndexed(indexCount, startIdx, vertexOffset);					//시작인덱스, 인덱스버퍼의 시작번호, 버텍스버퍼의 오프셋값
}

ID3D11Device* Direct3D::GetDevice()
{
	return m_pD3D_Device;
}

ID3D11DeviceContext* Direct3D::GetDeviceContext()
{
	return m_pD3D_DeviceContext;
}
