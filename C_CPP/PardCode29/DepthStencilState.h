#pragma once
#include "CommonHeader.h"
#include "BaseState.h"

class DepthStencilState : public BaseState<ID3D11DepthStencilState>
{
public:
	DepthStencilState(ID3D11Device* pDevice);
	~DepthStencilState();
	DepthStencilState(const DepthStencilState&) = delete;
	DepthStencilState& operator=(const DepthStencilState&) = delete;
	DepthStencilState(DepthStencilState&&) = delete;
	DepthStencilState& operator=(DepthStencilState&&) = delete;

	void SetDS(ID3D11DeviceContext* pDeviceContext, E_DSStates state, UINT stencilRef = 1);
private:
};

