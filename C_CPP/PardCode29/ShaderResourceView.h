#pragma once
#include "BaseView.h"

class ShaderResourceView : public BaseView
{
public:
	ShaderResourceView(ID3D11Device* pDevice, const ScratchImage* resource);
	virtual ~ShaderResourceView();
	ShaderResourceView(const ShaderResourceView&) = delete;
	ShaderResourceView& operator=(const ShaderResourceView&) = delete;
	ShaderResourceView(ShaderResourceView&&) = delete;
	ShaderResourceView& operator=(ShaderResourceView&&) = delete;

	void SetVS(ID3D11DeviceContext* pDeviceContext, UINT startIdx = 0);
	void SetPS(ID3D11DeviceContext* pDeviceContext, UINT startIdx = 0);
private:
	ID3D11ShaderResourceView* m_pSRV;
};
