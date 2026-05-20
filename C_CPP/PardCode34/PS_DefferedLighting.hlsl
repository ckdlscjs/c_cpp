#include "PS_Lights.hlsli"
#include "PS_Shadows.hlsli"
Texture2D GBufferTex_Position : register(t11);
Texture2D GBufferTex_Normal : register(t12);
Texture2D GBufferTex_Albedo : register(t13);
Texture2D GBufferTex_Specular : register(t14);
sampler TextureSampler : register(s0); //세팅하지않아도 기본샘플러가 레지스터0에 세팅된다

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 pos1 : WORLDP0;
};

cbuffer CB_Campos : register(b5)
{
    float4 campos;
};

cbuffer CB_LightMat : register(b6)
{
    row_major float4x4 matLightView;
    row_major float4x4 matLightProj;
    float4 LightPos;
};

float4 psmain(PS_INPUT input) : SV_Target
{
    
    float4 GBuffer_Position = GBufferTex_Position.Sample(TextureSampler, input.tex0.xy);
    float4 GBuffer_Normal = GBufferTex_Normal.Sample(TextureSampler, input.tex0.xy);
    float4 GBuffer_Albedo = GBufferTex_Albedo.Sample(TextureSampler, input.tex0.xy);
    float4 GBuffer_Specular = GBufferTex_Specular.Sample(TextureSampler, input.tex0.xy);
    
    float4 P = float4(GBuffer_Position.xyz, 1.0f);  //GBuffer에 저장된 월드값
    float3 N = normalize(GBuffer_Normal.xyz);       //GBuffer에 저장된 법선 정규화
    
    //그림자맵 계산을 위한 광원 시점 행렬의 변환
    float4 LP = mul(P, matLightView);
    LP = mul(LP, matLightProj);
    //투영 텍스쳐링을 위해 광원시점으로의 변환된 값을 텍스쳐좌표계로 변환, 그림자값계산
    float shadowFactor = CalcShadowFactor(LP, TextureSampler);
    
    //DirectionalLight
    float3 L = normalize(-ld_dir); //광원을향하는벡터
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
    
    float4 lightIntensity = DL.intensity.Ambient + (DL.intensity.Diffuse + DL.intensity.Specular) * shadowFactor;
    return saturate(lightIntensity * GBuffer_Albedo); //마지막에 텍스쳐컬러를 곱해도 분배법칙에의해 같은결과가 나온다
}
