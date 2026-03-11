#pragma once

class LightClass : public AlignedAllocationPolicy<16>
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetPosition(float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetPosition();

private:
	XMFLOAT4 m_ambientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 m_diffuseColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
};