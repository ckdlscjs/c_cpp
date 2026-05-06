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
	void OnResize(UINT width, UINT height);

	/////////////////////////////
	//API Usage
	/////////////////////////////
private:
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

	/////////////////////////////
	//Render Pass
	/////////////////////////////
public:
	uint32_t GetRenderPassKey_Shaders(size_t hashMaterial);
	uint32_t GetRenderPassKey_States(E_RSState stateRS, E_DSState stateDS, E_BSState stateBS, UINT ds_stencilref = 0, float* bs_factor = nullptr, UINT bs_mask = 0xFFFFFFFF);
	uint32_t GetRenderPassKey_Resources(size_t hashMesh, size_t hashMat, E_Collider collider, UINT idx = 127);
	uint32_t GetRenderPassKey_DistToCamera(float dist);
	_RPKey GenerateRenderPassHash(uint32_t hashPass, uint32_t hashShaders, uint32_t hashStates, uint32_t hashResources, uint32_t hashDist);
private:
	size_t GetHashMat_Debug(E_Collider eType);
	size_t GetHashMat_Cubemap(E_VerticesType eType);
	size_t GetHashMat_Outline(E_VerticesType eType);
	void EnqueueRenderItem(_RPKey sortKey, Archetype* pArchetype, size_t entityRow, size_t entityCol, UINT renderCnt, UINT startIdx);
	void SortRenderItem();
	void ClearRenderItem();
	void CollectRenderItem(const Vector3& posCam);

	//RenderPass
	std::unordered_map<size_t, uint16_t>				m_hRP_Shaders;
	std::unordered_map<size_t, uint8_t>					m_hRP_States;
	std::unordered_map<size_t, uint16_t>				m_hRP_Resources;
	std::vector<size_t>									m_resRP_Shaders;
	std::vector<RPStates>								m_resRP_States;
	std::vector<RPResources>							m_resRP_Resources;
	std::vector<RenderItem>								m_hRP_CommandQueue;		//수집후 sort

	//최적화 멤버변수
	ID3D11InputLayout* m_Inputlayout;
	ID3D11Buffer* m_VB;
	ID3D11Buffer* m_IB;
	ID3D11VertexShader* m_VS;
	ID3D11HullShader* m_HS;
	ID3D11DomainShader* m_DS;
	ID3D11GeometryShader* m_GS;
	ID3D11PixelShader* m_PS;
	ID3D11SamplerState* m_SamplerState;
	ID3D11BlendState* m_BlendState;
	ID3D11DepthStencilState* m_DepthStencilState;
	ID3D11RasterizerState* m_RasterizerState;
};
//SingletonClasses
#define _RenderSystem RenderSystem::GetInstance()
