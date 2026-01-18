#include "PS_Lights.hlsli"

Texture2D Texture : register(t0);       //psset으로세팅하였다
Texture2D ShadowMapTexture : register(t6); //psset으로세팅하였다

sampler TextureSampler : register(s0);  //세팅하지않아도 기본샘플러가 레지스터0에 세팅된다

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 pos1 : WORLDP0;
    float4 lightPos0 : TEXCOORD1;
    float3 lightNormal0 : NORMAL1;
};

cbuffer CB_Campos : register(b5)
{
    float4 campos;
};

float4 psmain(PS_INPUT input) : SV_Target
{
    float4 M = Texture.Sample(TextureSampler, input.tex0.xy); //텍스쳐값
    float4 P = input.pos1; //변환된 월드의정점
    clip(M.a - 0.5f);
    

    //투영 텍스쳐링을 위해 광원시점으로의 변환된 값을 텍스쳐좌표계로 변환한다
    float3 ShadowMapNDC = input.lightPos0.xyz / input.lightPos0.w;
    float2 ShadowTexCoord;
    ShadowTexCoord.x = ShadowMapNDC.x * 0.5f + 0.5f;
    ShadowTexCoord.y = -ShadowMapNDC.y * 0.5f + 0.5f;

    float shadowFactor = 1.0f; //1.0f->그림자없음
    
    // 투영 된 좌표가 0에서 1 범위에 있는지 결정합니다. 그렇다면이 픽셀은 빛의 관점에 있습니다.
    if ((saturate(ShadowTexCoord.x) == ShadowTexCoord.x) && (saturate(ShadowTexCoord.y) == ShadowTexCoord.y))
    {
        float shadowBias = 0.001f;
        
        // 빛의 깊이를 계산합니다.
        float lightDepthValue = ShadowMapNDC.z;
        
		// 투영 된 텍스처 좌표 위치에서 샘플러를 사용하여 깊이 텍스처에서 섀도우 맵 깊이 값을 샘플링합니다.
        float ShadowDepthValue = ShadowMapTexture.Sample(TextureSampler, ShadowTexCoord).r;

		// 섀도우 맵 값의 깊이와 빛의 깊이를 비교하여 이 픽셀을 음영 처리할지 조명할지 결정합니다.
		// 빛이 객체 앞에 있으면 픽셀을 비추고, 그렇지 않으면 객체 (오클 루더)가 그림자를 드리 우기 때문에 이 픽셀을 그림자로 그립니다.
        if (lightDepthValue > ShadowDepthValue + shadowBias)
        {
            shadowFactor = 0.5f;
        }
    }
    
    
    //DirectionalLight
    float3 L = normalize(-ld_dir); //광원을향하는벡터
    float3 N = normalize(input.normal0.xyz); //정점법선벡터 정규화
    float3 R = reflect(ld_dir, N); //반사벡터(입사벡터, 법선을 인자로사용)
    float3 V = normalize(campos.xyz - P.xyz); //시점을향하는벡터
    Calc_DL DL = DirectionalLight(L, N, R, V);
    
    ////PointLight
    //float3 L = lp_pos - P.xyz; //점광, 정점에서 광원을향하는방향
    //float D = length(L);
    //if (D > lp_range)                                           //빛의 범위보다 멀리있을경우
    //    return float4(0.0f, 0.0f, 0.0f, 0.0f);
    //L /= D; //정규화
    //float3 N = normalize(input.normal0.xyz);
    //float3 R = reflect(-L, N); //빛의 입사벡터(-L)
    //float3 V = normalize(campos.xyz - P.xyz);
    //return PointLight(L, D, N, R, V) * M;
    
    
    ////SpotLight
    //float3 L = ls_pos - P.xyz;
    //float D = length(L);
    //if (D > ls_range)
    //    return float4(0.0f, 0.0f, 0.0f, 0.0f);
    //L /= D;
    //float3 N = normalize(input.normal0.xyz);
    //float3 R = reflect(-L, N);
    //float3 V = normalize(campos.xyz - P.xyz);
    //return SpotLight(L, D, N, R, V) * M;
    
    float4 lightIntensity = (DL.intensity.Diffuse + DL.intensity.Specular) * shadowFactor;
    
    return saturate(lightIntensity) * M; //마지막에 텍스쳐컬러를 곱해도 분배법칙에의해 같은결과가 나온다
    
}