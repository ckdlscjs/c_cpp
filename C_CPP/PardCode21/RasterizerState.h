#pragma once
#include "CommonHeader.h"

class RasterizerState
{
public:
	RasterizerState(ID3D11Device* pDevice);
	~RasterizerState();
	RasterizerState(const RasterizerState&) = delete;
	RasterizerState& operator=(const RasterizerState&) = delete;
	RasterizerState(RasterizerState&&) = delete;
	RasterizerState& operator=(RasterizerState&&) = delete;
	void SetRS(ID3D11DeviceContext* pDeviceContext, UINT idx);
private:
	std::vector<ID3D11RasterizerState*> m_pStates;
};
