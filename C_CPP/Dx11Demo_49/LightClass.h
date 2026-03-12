#pragma once

class LightClass : public AlignedAllocationPolicy<16>
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetDirection(XMFLOAT3);
	void SetPosition(XMFLOAT3);
	void SetLookAt(XMFLOAT3);

	XMFLOAT3 GetDirection();
	XMFLOAT3 GetPosition();

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);
	void GenerateOrthoMatrix(float, float, float, float);

	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

private:
	XMFLOAT3 m_direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_orthoMatrix;
};