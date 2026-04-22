#pragma once
#include "BaseSystem.h"

class RenderSystem : public BaseSystem<RenderSystem>
{
	friend class BaseSystem<RenderSystem>;
private:
	RenderSystem();
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;
	RenderSystem(RenderSystem&&) = delete;
	RenderSystem& operator=(RenderSystem&&) = delete;
public:
	~RenderSystem();
	void Init();
	void Frame(float deltatime, float elapsedtime);
	void PreRender(float deltatime, float elapsedtime);
	void Render(float deltatime, float elapsedtime);
	void PostRender();

	/////////////////////////////
	//API Usage
	/////////////////////////////
public:
	void OnResize(UINT width, UINT height);
	void SetViewportSize(D3D11_VIEWPORT* pViewport, UINT iWidth, UINT iHeight);
	void ClearRenderTargetView(size_t hashRTV, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);
	void ClearDepthStencilView(size_t hashDSV);

	void SetIA_Topology(D3D_PRIMITIVE_TOPOLOGY topology);
	void SetIA_InputLayout(size_t hashIL);
	void SetIA_VertexBuffer(size_t hashVB, UINT offset = 0);
	void SetIA_IndexBuffer(size_t hashIB, UINT offset = 0);

	void SetVS_Shader(size_t hashVS);
	void SetVS_ShaderResourceView(size_t hashSRV, UINT startIdx = 0);
	void SetVS_ConstantBuffer(size_t hashCB, UINT startIdx = 0);
	void SetVS_SamplerState(E_SMState eSampler, UINT startIdx = 0);

	void SetHS_Shader(size_t hashHS);
	void SetHS_ConstantBuffer(size_t hashCB, UINT startIdx = 0);

	void SetDS_Shader(size_t hashDS);
	void SetDS_ConstantBuffer(size_t hashCB, UINT startIdx = 0);

	void SetGS_Shader(size_t hashGS);
	void SetGS_ConstantBuffer(size_t hashCB, UINT startIdx = 0);

	void SetPS_Shader(size_t hashPS);
	void SetPS_ShaderResourceView(size_t hashSRV, UINT startIdx = 0);
	void SetPS_ConstantBuffer(size_t hashCB, UINT startIdx = 0);
	void SetPS_SamplerState(E_SMState eSampler, UINT startIdx = 0);

	void SetRS_RasterizerState(E_RSState eRSState);
	void SetRS_Viewport(D3D11_VIEWPORT* pViewport);

	void SetOM_RenderTargets(std::vector<size_t> hashRtvs, size_t hashDSV);
	void SetOM_DepthStenilState(E_DSState eDSState, UINT stencilRef = 0);
	void SetOM_BlendState(E_BSState eBSState, const FLOAT* blendFactor, UINT sampleMask = 0xFFFFFFFF);

	void Draw_Vertices(UINT vertexCount, UINT startIdx);
	void Draw_Indicies(UINT indexCount, UINT startIdx, INT vertexOffset);

	void SwapchainPresent(bool vsync);

	//RenderPass
private:
	void RenderSkySphere(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderGeometry(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderBillboard(const Vector3& campos, const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderShadowMap(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderCubeMap();
	void RenderUI(const Matrix4x4& matOrtho);
	void RenderGeometry_Debug(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderGeometry_PickingTriangle(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderGeometry_PickingOutline(const Matrix4x4& matView, const Matrix4x4& matProj);

	size_t GetHashMat_Debug(E_Collider eType);
	size_t GetHashMat_Cubemap(E_VerticesType eType);
	size_t GetHashMat_Outline(E_VerticesType eType);
	void CollectRenderItem(const Vector3& posCam);
	void CollectSkySphere(const Matrix4x4& matView, const Matrix4x4& matProj);
	void CollectGeometry(const Matrix4x4& matView, const Matrix4x4& matProj);
	void CollectBillboard(const Vector3& campos, const Matrix4x4& matView, const Matrix4x4& matProj);
	void CollectShadowMap(const Matrix4x4& matView, const Matrix4x4& matProj);
	void CollectCubeMap();
	void CollectUI(const Matrix4x4& matOrtho);
	void CollectGeometry_Debug(const Matrix4x4& matView, const Matrix4x4& matProj);
	void CollectGeometry_PickingTriangle(const Matrix4x4& matView, const Matrix4x4& matProj);
	void CollectGeometry_PickingOutline(const Matrix4x4& matView, const Matrix4x4& matProj);
};
//SingletonClasses
#define _RenderSystem RenderSystem::GetInstance()
