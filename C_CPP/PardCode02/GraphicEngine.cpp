#include "GraphicEngine.h"
#include <iostream>

GraphicEngine::GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << " ������ ȣ��" << '\n';
}

GraphicEngine::~GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << "�Ҹ��� ȣ��" << '\n';
}

bool GraphicEngine::Init()
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
	for (UINT driver_type_idx = 0; driver_type_idx < num_drivers; driver_type_idx++)
	{
		if (SUCCEEDED(D3D11CreateDevice
		(
			NULL,								//_In_opt_ IDXGIAdapter * pAdapter, ���÷��� ���������, 0(NULL)�� �⺻ ���÷��� ����� ���
			driver_types[driver_type_idx],		//D3D_DRIVER_TYPE DriverType, �������� ��� �׷��� ���� ����̹��� �����ΰ� GPU�� �⺻���εд�
			NULL,								//HMODULE Software, �ϵ�������ϹǷ�null�εд�
			NULL,								//UINT Flags, �߰��� ��ġ ���� �÷��׵�, �Ϲ������� �ΰ��� �÷��׸� ������ ����Ѵ�(Debug�÷���, �̱۽����� ���� �����÷���)
			feature_levels,						//_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL * pFeatureLevels, ���ķ��� �迭
			num_features,						//UINT FeatureLevels, ���ķ����ǰ���
			D3D11_SDK_VERSION,					//UINT SDKVersion, sdk��������
			&m_D3DDevice,						//_COM_Outptr_opt_ ID3D11Device * *ppDevice, ��ȯ�� ID3DDevice��ü
			&m_FeatureLevel,					//_Out_opt_ D3D_FEATURE_LEVEL * pFeatureLevel, ��ȯ��FeatureLevel
			&m_D3DDeviceContext					//_COM_Outptr_opt_ ID3D11DeviceContext * *ppImmediateContext, ��ȯ��ID3DDeviceContext��ü
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

//�̱��水ü, static�������� ������Ÿ�ӿ� ��ü�� �����ȴ�
GraphicEngine& GraphicEngine::Get()
{
	static GraphicEngine engine;
	return engine;
}
