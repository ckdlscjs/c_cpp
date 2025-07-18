#include "Direct3D.h"
#include "SwapChain.h"
Direct3D::Direct3D()
{
	std::cout << "Direct3D" << " Class" << " ������ ȣ��" << '\n';
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
	std::cout << "Direct3D" << " Class" << "�Ҹ��� ȣ��" << '\n';
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
	m_pD3D_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//Vertex�� TrangleList�α׸���
	m_pD3D_DeviceContext->Draw(vertexCount, startIdx);										//�Է¹��� vertex_count, ���۹�ȣ
}

void Direct3D::DrawVertex_TriangleStrip(UINT vertexCount, UINT startIdx)
{
	m_pD3D_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	//Vertex�� TrangleStrip
	m_pD3D_DeviceContext->Draw(vertexCount, startIdx);
}

void Direct3D::DrawIndex_TriagleList(UINT indexCount, UINT startIdx, INT vertexOffset)
{
	m_pD3D_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//Index�� TrangleList�α׸���
	m_pD3D_DeviceContext->DrawIndexed(indexCount, startIdx, vertexOffset);					//�����ε���, �ε��������� ���۹�ȣ, ���ؽ������� �����°�
}

void Direct3D::DrawIndex_TriangleStrip(UINT indexCount, UINT startIdx, INT vertexOffset)
{
	m_pD3D_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	//Index�� TrangleStrip
	m_pD3D_DeviceContext->DrawIndexed(indexCount, startIdx, vertexOffset);					//�����ε���, �ε��������� ���۹�ȣ, ���ؽ������� �����°�
}

ID3D11Device* Direct3D::GetDevice()
{
	return m_pD3D_Device;
}

ID3D11DeviceContext* Direct3D::GetDeviceContext()
{
	return m_pD3D_DeviceContext;
}
