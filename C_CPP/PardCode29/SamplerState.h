#pragma once
#include "BaseState.h"

class SamplerState : public BaseState<ID3D11SamplerState>
{
public:
	SamplerState(ID3D11Device* pDevice);
	~SamplerState();
	SamplerState(const SamplerState&) = delete;
	SamplerState& operator=(const SamplerState&) = delete;
	SamplerState(SamplerState&&) = delete;
	SamplerState& operator=(SamplerState&&) = delete;
	void SetVS(ID3D11DeviceContext* pDeviceContext, E_Samplers sampler, UINT startIdx = 0);
	void SetPS(ID3D11DeviceContext* pDeviceContext, E_Samplers sampler, UINT startIdx = 0);
};
