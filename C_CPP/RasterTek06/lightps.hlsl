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

	// 계산을 위해 빛 방향을 반전시킵니다->정점에서 광원을 향하게한다
    lightDir = -lightDirection;

    // 이 픽셀의 빛의 양을 계산합니다.
    /*
    HLSL에서 saturate 함수는 입력값을 [0, 1] 범위 내에 유지합니다. 
    입력값이 0보다 작으면 0으로, 1보다 크면 1로 클램핑합니다. 
    즉, saturate(x) = min(max(x, 0), 1.0f)입니다. 
    이 함수는 일반적으로 컬러 값이나 거리 계산과 같은 다양한 그래픽 알고리즘에서 사용됩니다.
    */
    //Kd -> min(0.0f, max(L dot N, 0.0f))
    lightIntensity = saturate(dot(input.normal, lightDir));

    // 빛의 강도와 결합 된 확산 색을 기준으로 최종 색상의 최종 색상을 결정합니다. -> Kd * Ld
    color = saturate(diffuseColor * lightIntensity);

    // 텍스처 픽셀과 최종 확산 색을 곱하여 최종 픽셀 색상 결과를 얻습니다. -> Kd * Ld * Md
	color = lightIntensity;

    return color;
}