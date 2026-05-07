#include "PS_Lights.hlsli"
#include "PS_Shadows.hlsli"
Texture2D TextureDiffuse : register(t0);
Texture2D TextureNormal : register(t1);
Texture2D TextureSpecular : register(t2);
sampler TextureSampler : register(s0); //세팅하지않아도 기본샘플러가 레지스터0에 세팅된다

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 tangent0 : TANGENT0;
    float4 binormal0 : BINORMAL0;
     //LightResources
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
    float4 M_Diffuse = TextureDiffuse.Sample(TextureSampler, input.tex0.xy); //텍스쳐값
    float4 M_Normal = TextureNormal.Sample(TextureSampler, input.tex0.xy); //텍스쳐값
    M_Normal = (2.0f * M_Normal) - 1.0f; //(0 ~ 1) -> (-1 ~ 1) 정규화
    float4 P = input.pos1; //변환된 월드의정점
    clip(M_Diffuse.a - 0.5f);
    //float nx = M_Normal.x * input.tangent0.x + M_Normal.y * input.binormal0.x + M_Normal.z * input.normal0.x;                    
    //float ny = M_Normal.x * input.tangent0.y + M_Normal.y * input.binormal0.y + M_Normal.z * input.normal0.y;
    //float nz = M_Normal.x * input.tangent0.z + M_Normal.y * input.binormal0.z + M_Normal.z * input.normal0.z;
    float3 convert_normal = (M_Normal.x * input.tangent0) + (M_Normal.y * input.binormal0) + (M_Normal.z * input.normal0);
    convert_normal = normalize(convert_normal);
    
    //투영 텍스쳐링을 위해 광원시점으로의 변환된 값을 텍스쳐좌표계로 변환
    float shadowFactor = CalcShadowFactor(input.lightPos0, TextureSampler);
    
    //DirectionalLight
    float3 L = normalize(-ld_dir); //광원을향하는벡터
    float3 N = normalize(convert_normal); //정점법선벡터 정규화
    float3 R = reflect(ld_dir, N); //반사벡터(입사벡터, 법선을 인자로사용)
    float3 V = normalize(campos.xyz - P.xyz); //시점을향하는벡터
    Calc_DL DL = DirectionalLight(L, N, R, V);
    float4 intensity = DL.intensity.Ambient + (DL.intensity.Diffuse + DL.intensity.Specular) * shadowFactor;
    return saturate(intensity * M_Diffuse); //마지막에 텍스쳐컬러를 곱해도 분배법칙에의해 같은결과가 나온다
    
    
    ////PointLight
    //float3 L = lp_pos - P.xyz; //점광, 정점에서 광원을향하는방향
    //float D = length(L);
    //if (D > lp_range)                                           //빛의 범위보다 멀리있을경우
    //    return float4(0.0f, 0.0f, 0.0f, 0.0f);
    //L /= D; //정규화
    //float3 N = normalize(convert_normal);
    //float3 R = reflect(-L, N); //빛의 입사벡터(-L)
    //float3 V = normalize(campos.xyz - P.xyz);
    //return PointLight(L, D, N, R, V) * M_Diffuse;
    
    
    ////SpotLight
    //float3 L = ls_pos - P.xyz;
    //float D = length(L);
    //if (D > ls_range)
    //    return float4(0.0f, 0.0f, 0.0f, 0.0f);
    //L /= D;
    //float3 N = normalize(convert_normal);
    //float3 R = reflect(-L, N);
    //float3 V = normalize(campos.xyz - P.xyz);
    //return SpotLight(L, D, N, R, V) * M_Diffuse;
}