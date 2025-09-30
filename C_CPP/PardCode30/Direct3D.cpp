#include "Direct3D.h"
#include "SwapChain.h"
Direct3D::Direct3D()
{
	std::cout << "Initialize : " << "Direct3D" << " Class" << '\n';
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
			&m_D3D_FeatureLevel,				//_Out_opt_ D3D_FEATURE_LEVEL * pFeatureLevel, ��ȯ��FeatureLevel
			&m_pD3D_DeviceContext				//_COM_Outptr_opt_ ID3D11DeviceContext * *ppImmediateContext, ��ȯ��ID3DDeviceContext��ü
		);
		if (SUCCEEDED(hResult))
			break;
	}
	_ASEERTION_CREATE(hResult, "Device&DeviceContext");
}

Direct3D::~Direct3D()
{
	std::cout << "Release : " << "Direct3D" << " Class" << '\n';
	m_pD3D_Device->Release();
	m_pD3D_DeviceContext->Release();
}

ID3D11Device* Direct3D::GetDevice()
{
	return m_pD3D_Device;
}

ID3D11DeviceContext* Direct3D::GetDeviceContext()
{
	return m_pD3D_DeviceContext;
}
