#pragma once
#include "std.h"
class SamplerState
{
public:
	SamplerState(ID3D11Device* pDevice);
	~SamplerState();
	SamplerState(const SamplerState&) = delete;
	SamplerState& operator=(const SamplerState&) = delete;
	SamplerState(SamplerState&&) = delete;
	SamplerState& operator=(SamplerState&&) = delete;
	void SetVS(ID3D11DeviceContext* pDeviceContext, Samplers sampler, UINT startIdx = 0);
	void SetPS(ID3D11DeviceContext* pDeviceContext, Samplers sampler, UINT startIdx = 0);
private:
	std::vector<ID3D11SamplerState*> m_pSamplers;
};
