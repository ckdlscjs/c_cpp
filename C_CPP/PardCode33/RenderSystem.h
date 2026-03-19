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

	//RenderPass
private:
	void RenderSkySphere(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderGeometry(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderBillboard(const Vector3& campos, const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderShadowMap(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderCubeMap();
	void RenderUI(const Matrix4x4& matOrtho);
	void RenderGeometry_Debug(const Matrix4x4& matView, const Matrix4x4& matProj);
	void RenderGeometry_Picking(const Matrix4x4& matView, const Matrix4x4& matProj);
public:
};
//SingletonClasses
#define _RenderSystem RenderSystem::GetInstance()
