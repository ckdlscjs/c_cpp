#pragma once
#include "CommonHeader.h"

class ShaderResourceView
{
public:
	ShaderResourceView();
	ShaderResourceView(ID3D11Device* pDevice, const ScratchImage* resource);
	virtual ~ShaderResourceView();
	ShaderResourceView(const ShaderResourceView&) = delete;
	ShaderResourceView& operator=(const ShaderResourceView&) = delete;
	ShaderResourceView(ShaderResourceView&&) = delete;
	ShaderResourceView& operator=(ShaderResourceView&&) = delete;
	ID3D11ShaderResourceView* GetSRV();
	void SetVS(ID3D11DeviceContext* pDeviceContext, UINT startIdx = 0);
	void SetPS(ID3D11DeviceContext* pDeviceContext, UINT startIdx = 0);
protected:
	ID3D11ShaderResourceView* m_pSRV;
};
