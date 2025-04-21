#include "GraphicEngine.h"
#include "SwapChain.h"
#include "Window.h"

GraphicEngine::GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << " ������ ȣ��" << '\n';
}

GraphicEngine::~GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << "�Ҹ��� ȣ��" << '\n';
}

bool GraphicEngine::Init(Window* pWindow)
{
	//����̹�Ÿ��
	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,			//�ϵ������, ��������������
		D3D_DRIVER_TYPE_WARP,				//�߰�
		D3D_DRIVER_TYPE_SOFTWARE,			//����Ʈ������, ���������Ͼ�����
	};
	UINT num_drivers = ARRAYSIZE(driver_types); //sizeof

	//���ķ���
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
			NULL,								//_In_opt_ IDXGIAdapter * pAdapter, ���÷��� ���������, 0(NULL)�� �⺻ ���÷��� ����� ���
			driver_types[driver_type_idx],		//D3D_DRIVER_TYPE DriverType, �������� ��� �׷��� ���� ����̹��� �����ΰ� GPU�� �⺻���εд�
			NULL,								//HMODULE Software, �ϵ�������ϹǷ�null�εд�
			NULL,								//UINT Flags, �߰��� ��ġ ���� �÷��׵�, �Ϲ������� �ΰ��� �÷��׸� ������ ����Ѵ�(Debug�÷���, �̱۽����� ���� �����÷���)
			feature_levels,						//_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL * pFeatureLevels, ���ķ��� �迭
			num_features,						//UINT FeatureLevels, ���ķ����ǰ���
			D3D11_SDK_VERSION,					//UINT SDKVersion, sdk��������
			&m_pD3D_Device,						//_COM_Outptr_opt_ ID3D11Device * *ppDevice, ��ȯ�� ID3DDevice��ü
			&m_D3D_FeatureLevel,					//_Out_opt_ D3D_FEATURE_LEVEL * pFeatureLevel, ��ȯ��FeatureLevel
			&m_pD3D_DeviceContext					//_COM_Outptr_opt_ ID3D11DeviceContext * *ppImmediateContext, ��ȯ��ID3DDeviceContext��ü
		);
		if (SUCCEEDED(hResult))
			break;
	}
	if (FAILED(hResult))
		return false;
	//D3DDevice���� Factory���� �������� �����ؼ� Factory��ü���� �ּҸ� �޾ƿ´�
	//__uuidof�� �ش� �Լ����� �޾ƿ� ��ü�� ID�� �ǹ��ϸ� �ι�°���ڷ� (void**)�� ĳ������ �������� �������� �ּҰ��� �Ѱܾ� ���� m_������ ���������س������ִ�
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

//�̱��水ü, static�������� ������Ÿ�ӿ� ��ü�� �����ȴ�
GraphicEngine& GraphicEngine::Get()
{
	static GraphicEngine engine;
	return engine;
}
