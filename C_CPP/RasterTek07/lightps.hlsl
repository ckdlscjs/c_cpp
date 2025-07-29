////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;


	// �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// ����� ���� �� ������ ������ŵ�ϴ�.
    lightDir = -lightDirection;

    // �� �ȼ��� ���� ���� ����մϴ�.
    lightIntensity = saturate(dot(input.normal, lightDir));

    //if (0.0f < lightIntensity && lightIntensity <= 0.3f)
    //{
    //    lightIntensity = 0.15f;
    //}
    //else if (0.3f < lightIntensity && lightIntensity <= 0.6f)
    //{
    //    lightIntensity = 0.45f;
    //}
    //else if (0.6f < lightIntensity && lightIntensity <= 0.85f)
    //{
    //    lightIntensity = 0.7f;
    //}
    //else if (0.7f < lightIntensity)
    //{
    //    lightIntensity = 0.95f;
    //}
    // ���� ������ ���� �� Ȯ�� ���� �������� ���� ������ ���� ������ �����մϴ�.
    color = saturate(diffuseColor * lightIntensity);

    // �ؽ�ó �ȼ��� ���� Ȯ�� ���� ���Ͽ� ���� �ȼ� ���� ����� ����ϴ�.
    color = color * textureColor;

    return color;
}