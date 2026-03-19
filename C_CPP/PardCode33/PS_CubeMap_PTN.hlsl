#include "PS_Lights.hlsli"
#include "PS_Shadows.hlsli"

Texture2D Texture : register(t0);                   
TextureCube CubeTexture : register(t7);             
SamplerState TextureSampler : register(s0);         

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;                      //로컬정점
    float4 tex0 : TEXCOORD0;                        //텍스쳐좌표
    float4 normal0 : NORMAL0;                       //노말값
    uint rtvIdx : SV_RenderTargetArrayIndex;        // GS에서 넘겨준 면 인덱스, 사용되지는않으나 시맨틱일치를 위한 선언이필요
    
    //LightResources
    float4 pos1 : WORLDP0; //월드변환된정점
    float4 lightPos0 : TEXCOORD1;
    float3 lightNormal0 : NORMAL1;
};

cbuffer CB_Campos : register(b5)
{
    float4 campos;                                  //카메라(시점)의 위치
};

float4 psmain(PS_INPUT input) : SV_Target
{
    float4 M_Diffuse = Texture.Sample(TextureSampler, input.tex0.xy); //텍스쳐값
    
    float4 P = input.pos1;                          //변환된 월드의정점
    float3 L = normalize(-ld_dir);                  //광원을향하는벡터
    float3 N = normalize(input.normal0.xyz);        //정점법선벡터 정규화
    float3 R = reflect(ld_dir, N);                  //반사벡터(입사벡터, 법선을 인자로사용)
    float3 V = normalize(campos - P);               //시점을향하는벡터
    
    float3 incident = -V;                           //시점에서 픽셀을 향하는벡터
    float3 reflectionV = reflect(incident, N);      //픽셀을기준으로 반사벡터를 계산해 texuv로활용
    float4 M_CubeMap = CubeTexture.Sample(TextureSampler, reflectionV);
    
    //투영 텍스쳐링을 위해 광원시점으로의 변환된 값을 텍스쳐좌표계로 변환
    float shadowFactor = CalcShadowFactor(input.lightPos0, TextureSampler);
    
    Calc_DL DL = DirectionalLight(L, N, R, V);
    float4 intensity = DL.intensity.Ambient + (DL.intensity.Diffuse + DL.intensity.Specular) * shadowFactor;
    return saturate(intensity * M_Diffuse * M_CubeMap);
}