#include "PS_Lights.hlsli"
Texture2D TextureDiffuse : register(t0); //psset으로세팅하였다
sampler TextureSampler : register(s0); //세팅하지않아도 기본샘플러가 레지스터0에 세팅된다

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;

    //LightResources
    float4 pos1 : WORLDP0;
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

Gbuffer_Output psmain(PS_INPUT input)
{
   
    float4 M_Diffuse = TextureDiffuse.Sample(TextureSampler, input.tex0.xy); //텍스쳐값
    float4 P = input.pos1; //변환된 월드의정점

    clip(M_Diffuse.a - 0.1f);

    Gbuffer_Output output;
    output.gbuffer_Position = float4(input.pos1.xyz, 1.0f);
    output.gbuffer_Normal = float4(input.normal0.xyz, 0.0f); //R16G16B16A16 FLOAT 이므로 -1~1그대로사용, UNORM일경우 언더플로우되므로 0~1로변환이 필요함
    output.gbuffer_Albedo = M_Diffuse;
    output.gbuffer_Specular = float4(ld_specular.xyz, ld_shiness);
    return output;
}
