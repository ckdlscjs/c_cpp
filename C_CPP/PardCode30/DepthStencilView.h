#pragma once
#include "ShaderResourceView.h"

class DepthStencilView : public ShaderResourceView
{
public:
	DepthStencilView(ID3D11Device* pDevice, UINT width, UINT height);
	virtual ~DepthStencilView();
	DepthStencilView(const DepthStencilView&) = delete;
	DepthStencilView& operator=(const DepthStencilView&) = delete;
	DepthStencilView(DepthStencilView&&) = delete;
	DepthStencilView& operator=(DepthStencilView&&) = delete;
	ID3D11DepthStencilView* GetDSV();
	void Resize(ID3D11Device* pDevice, UINT width, UINT height);
private:
	ID3D11DepthStencilView* m_pDSV;
};
