#include "PS_Lights.hlsli"
Texture2D TextureDiffuse : register(t0);
Texture2D TextureNormal : register(t1);
TextureCube CubeTexture : register(t7);
SamplerState TextureSampler : register(s0);

struct PS_INPUT
{
    float4 pos0 : SV_POSITION; //로컬정점
    float4 tex0 : TEXCOORD0; //텍스쳐좌표
    float4 normal0 : NORMAL0; //노말값
    float4 tangent0 : TANGENT0;
    float4 binormal0 : BINORMAL0;
    uint rtvIdx : SV_RenderTargetArrayIndex; // GS에서 넘겨준 면 인덱스, 사용되지는않으나 시맨틱일치를 위한 선언이필요
    
    //LightResources
    float4 pos1 : WORLDP0; //월드변환된정점
    float4 lightPos0 : TEXCOORD1;
    float3 lightNormal0 : NORMAL1;
};

struct Gbuffer_Output
{
    float4 gbuffer_Position : SV_Target0;
    float4 gbuffer_Normal : SV_Target1;
    float4 gbuffer_Albedo : SV_Target2;
    float4 gbuffer_Specular : SV_Target3;
};

cbuffer CB_Campos : register(b5)
{
    float4 campos; //카메라(시점)의 위치
};

Gbuffer_Output psmain(PS_INPUT input)
{
    float4 M_Diffuse = TextureDiffuse.Sample(TextureSampler, input.tex0.xy); //텍스쳐값
    float4 M_Normal = TextureNormal.Sample(TextureSampler, input.tex0.xy); //텍스쳐값
    M_Normal = (2.0f * M_Normal) - 1.0f; //(0 ~ 1) -> (-1 ~ 1) 정규화
    clip(M_Diffuse.a - 0.5f);
    //float nx = M_Normal.x * input.tangent0.x + M_Normal.y * input.binormal0.x + M_Normal.z * input.normal0.x;                    
    //float ny = M_Normal.x * input.tangent0.y + M_Normal.y * input.binormal0.y + M_Normal.z * input.normal0.y;
    //float nz = M_Normal.x * input.tangent0.z + M_Normal.y * input.binormal0.z + M_Normal.z * input.normal0.z;
    float3 convert_normal = (M_Normal.x * input.tangent0) + (M_Normal.y * input.binormal0) + (M_Normal.z * input.normal0);
    convert_normal = normalize(convert_normal);
    
    
    
    float4 P = input.pos1; //변환된 월드의정점
    float3 N = normalize(input.normal0.xyz); //정점법선벡터 정규화
    float3 V = normalize(campos - P); //시점을향하는벡터
    
    float3 incident = -V; //시점에서 픽셀을 향하는벡터
    float3 reflectionV = reflect(incident, N); //픽셀을기준으로 반사벡터를 계산해 texuv로활용
    float4 M_CubeMap = CubeTexture.Sample(TextureSampler, reflectionV);
    
    Gbuffer_Output output;
    output.gbuffer_Position = float4(P.xyz, 1.0f);
    output.gbuffer_Normal = float4(N, 0.0f); //R16G16B16A16 FLOAT 이므로 -1~1그대로사용, UNORM일경우 언더플로우되므로 0~1로변환이 필요함
    output.gbuffer_Albedo = M_Diffuse * M_CubeMap;
    output.gbuffer_Specular = float4(ld_specular.xyz, ld_shiness);
    return output;
}
