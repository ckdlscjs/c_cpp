#pragma once
#include "BaseView.h"

class DepthStencilView : public BaseView
{
public:
	DepthStencilView(ID3D11Device* pDevice, UINT width, UINT height);
	virtual ~DepthStencilView();
	DepthStencilView(const DepthStencilView&) = delete;
	DepthStencilView& operator=(const DepthStencilView&) = delete;
	DepthStencilView(DepthStencilView&&) = delete;
	DepthStencilView& operator=(DepthStencilView&&) = delete;

private:
	ID3D11DepthStencilView* m_pDSV;
};
