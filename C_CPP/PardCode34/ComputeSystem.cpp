#include "ComputeSystem.h"
#include "EngineSystem.h"
#include "ECSSystem.h"

//Resources
#include "Shaders.h"
#include "Buffers.h"
#include "Views.h"

ComputeSystem::ComputeSystem()
{
}

ComputeSystem::~ComputeSystem()
{
}

void ComputeSystem::Init()
{
}

//프레임별 반복 수행연산(Blur, Bloom..등)
void ComputeSystem::Update()
{
}

void ComputeSystem::SetCS_Shader(size_t hashCS)
{
	auto pShader = _EngineSystem.GetCS(hashCS);
	auto ptr = pShader ? pShader->GetShader() : nullptr;
	_EngineSystem.GetD3DDeviceContext()->CSSetShader(ptr, nullptr, 0);
}

void ComputeSystem::SetCS_ShaderResourceView(size_t hashSRV, UINT startIdx)
{
	auto pView = _EngineSystem.GetSRV(hashSRV);
	auto ptr = pView ? pView->GetView() : nullptr;
	int isize = pView ? 1 : 0;
	_EngineSystem.GetD3DDeviceContext()->CSSetShaderResources(startIdx, isize, &ptr);
}

void ComputeSystem::SetCS_UnorderedAccessView(size_t hashUAV, UINT startIdx)
{
	auto pView = _EngineSystem.GetUAV(hashUAV);
	auto ptr = pView ? pView->GetView() : nullptr;
	int isize = pView ? 1 : 0;
	_EngineSystem.GetD3DDeviceContext()->CSSetUnorderedAccessViews(startIdx, isize, &ptr, 0);
}

void ComputeSystem::SetCS_ConstantBuffer(size_t hashCB, UINT startIdx)
{
	auto pBuffer = _EngineSystem.GetCB(hashCB);
	auto ptr = pBuffer ? pBuffer->GetBuffer() : nullptr;
	int isize = pBuffer ? 1 : 0;
	_EngineSystem.GetD3DDeviceContext()->CSSetConstantBuffers(startIdx, isize, &ptr);
}

//스레드그룹별 x,y,z
void ComputeSystem::Dispatch(UINT x, UINT y, UINT z)
{
	_EngineSystem.GetD3DDeviceContext()->Dispatch(x, y, z);
}
