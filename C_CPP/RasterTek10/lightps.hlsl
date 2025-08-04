////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture : register( t0 );
SamplerState SampleType : register( s0 );

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
    float3 lightDirection;
	float specularPower;
    float4 specularColor;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
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
	float3 reflection;
    float4 specular;


	// 이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// 모든 픽셀의 기본 출력 색상을 주변 광원 값으로 설정합니다.
    color = ambientColor;
	float3 normalizeLightDir = normalize(lightDirection);
	// 계산을 위해 빛 방향을 반전시킵니다.
	lightDir = -normalizeLightDir;

	// 이 픽셀의 빛의 양을 계산합니다.
    lightIntensity = saturate(dot(input.normal, lightDir));

	if(lightIntensity > 0.0f)
    {
        // 확산 색과 광 강도의 양에 따라 최종 확산 색을 결정합니다.
        color += (diffuseColor * lightIntensity);
    
    	// 최종 빛의 색상을 채 웁니다.
    	color = saturate(color);

		// 빛의 강도, 법선 벡터 및 빛의 방향에 따라 반사 벡터를 계산합니다.
		//dot(input.normal, lightDirection) -> 입사방향에따른 크기의 스칼라배값(법선의반대방향의 스칼라배)
		//위의결과 * inputNormal, 법선벡터를 반대방향의 그 길이만큼 스칼라배하여 길이벡터를 구한다,
		//입사벡터 - 2.0f(위의벡터) 하면 반사벡터를 구할 수 있다, 이를 정규화해 사용한다
		//Reflect = InputDir - 2.0f*(InputDir dot N)*N
		reflection = normalizeLightDir - 2.0f * dot(input.normal, normalizeLightDir) * input.normal;
		reflection = reflect(normalizeLightDir, input.normal);//hlsl의 반사벡터함수, 위와 내부적으로 동일하다
		// 반사 벡터, 시선 방향 및 반사 출력을 기준으로 반사 조명의 양을 결정합니다, 반사벡터와 시점을향하는 벡터의 일치도가 반사광의 세기가된다
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower) * lightIntensity;
	}

	// 텍스처 픽셀과 최종 확산 색을 곱하여 최종 픽셀 색상 결과를 얻습니다.
    color = color * textureColor;

	// 출력 색상의 마지막에 반사 컴포넌트를 추가합니다.
	color = saturate(color + specular);

    return color;
}