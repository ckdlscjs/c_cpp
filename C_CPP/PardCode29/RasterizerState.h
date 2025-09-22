#pragma once
#include "BaseState.h"

class RasterizerState : public BaseState<ID3D11RasterizerState>
{
public:
	RasterizerState(ID3D11Device* pDevice);
	~RasterizerState();
	RasterizerState(const RasterizerState&) = delete;
	RasterizerState& operator=(const RasterizerState&) = delete;
	RasterizerState(RasterizerState&&) = delete;
	RasterizerState& operator=(RasterizerState&&) = delete;
	void SetRS(ID3D11DeviceContext* pDeviceContext, E_RSStates rasterizer);
};
