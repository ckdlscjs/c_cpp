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


	// 이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// 계산을 위해 빛 방향을 반전시킵니다.
    lightDir = -lightDirection;

    // 이 픽셀의 빛의 양을 계산합니다.
    lightIntensity = saturate(dot(input.normal, lightDir));

    if (0.0f < lightIntensity && lightIntensity <= 0.3f)
    {
        lightIntensity = 0.15f;
    }
    else if (0.3f < lightIntensity && lightIntensity <= 0.6f)
    {
        lightIntensity = 0.45f;
    }
    else if (0.6f < lightIntensity && lightIntensity <= 0.85f)
    {
        lightIntensity = 0.7f;
    }
    else if (0.7f < lightIntensity)
    {
        lightIntensity = 0.95f;
    }
    // 빛의 강도와 결합 된 확산 색을 기준으로 최종 색상의 최종 색상을 결정합니다.
    color = saturate(diffuseColor * lightIntensity);

    // 텍스처 픽셀과 최종 확산 색을 곱하여 최종 픽셀 색상 결과를 얻습니다.
    color = color * textureColor;

    return color;
}