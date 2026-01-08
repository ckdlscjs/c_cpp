#include "PS_Lights.hlsli"

Texture2D Texture : register(t0);               //psset으로세팅하였다
TextureCube CubeTexture : register(t1);         //psset으로세팅하였다
SamplerState TextureSampler : register(s0);     //세팅하지않아도 기본샘플러가 레지스터0에 세팅된다

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;          //로컬정점
    float4 tex0 : TEXCOORD0;            //텍스쳐좌표
    float4 normal0 : NORMAL0;           //노말값
    float4 pos1 : WORLDP0;              //월드변환된정점
};

cbuffer CB_Campos : register(b5)
{
    float4 campos;                      //카메라(시점)의 위치
};

float4 psmain(PS_INPUT input) : SV_Target
{
    float4 P = input.pos1;                          //변환된 월드의정점
    float3 L = normalize(-ld_dir);                  //광원을향하는벡터
    float3 N = normalize(input.normal0.xyz);        //정점법선벡터 정규화
    float3 R = reflect(ld_dir, N);                  //반사벡터(입사벡터, 법선을 인자로사용)
    float3 V = normalize(campos - P);               //시점을향하는벡터
    float3 incident = -V;                           //시점에서 픽셀을 향하는벡터
    float3 reflectionV = reflect(incident, N);      //픽셀을기준으로 반사벡터를 계산해 texuv로활용
    return DirectionalLight(L, N, R, V) * Texture.Sample(TextureSampler, input.tex0.xy) * CubeTexture.Sample(TextureSampler, reflectionV);
}