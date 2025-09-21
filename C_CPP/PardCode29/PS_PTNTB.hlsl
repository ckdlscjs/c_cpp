#include "PS_Lights.hlsli"

Texture2D TextureDiffuse : register(t0);
Texture2D TextureNormal : register(t1);
sampler TextureSampler : register(s0); //���������ʾƵ� �⺻���÷��� ��������0�� ���õȴ�

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 tangent0 : TANGENT0;
    float4 binormal0 : BINORMAL0;
    float4 pos1 : WORLDP0;
};

cbuffer CB_Campos : register(b5)
{
    float4 campos;
};

float4 psmain(PS_INPUT input) : SV_Target
{
    float4 M_Diffuse = TextureDiffuse.Sample(TextureSampler, input.tex0.xy); //�ؽ��İ�
    float4 M_Normal = TextureNormal.Sample(TextureSampler, input.tex0.xy); //�ؽ��İ�
    M_Normal = (2.0f * M_Normal) - 1.0f; //(0 ~ 1) -> (-1 ~ 1) ����ȭ
    float4 P = input.pos1; //��ȯ�� ����������
    clip(M_Diffuse.a - 0.5f);
    //float nx = M_Normal.x * input.tangent0.x + M_Normal.y * input.binormal0.x + M_Normal.z * input.normal0.x;
    //float ny = M_Normal.x * input.tangent0.y + M_Normal.y * input.binormal0.y + M_Normal.z * input.normal0.y;
    //float nz = M_Normal.x * input.tangent0.z + M_Normal.y * input.binormal0.z + M_Normal.z * input.normal0.z;
    float3 convert_normal = (M_Normal.x * input.tangent0) + (M_Normal.y * input.binormal0) + (M_Normal.z * input.normal0);
    convert_normal = normalize(convert_normal);
    
    //DirectionalLight
    float3 L = normalize(-ld_dir); //���������ϴº���
    float3 N = normalize(convert_normal); //������������ ����ȭ
    float3 R = reflect(ld_dir, N); //�ݻ纤��(�Ի纤��, ������ ���ڷλ��)
    float3 V = normalize(campos.xyz - P.xyz); //���������ϴº���
    return DirectionalLight(L, N, R, V) * M_Diffuse; //�������� �ؽ����÷��� ���ص� �й��Ģ������ ��������� ���´�
    
    
    ////PointLight
    //float3 L = lp_pos - P.xyz; //����, �������� ���������ϴ¹���
    //float D = length(L);
    //if (D > lp_range)                                           //���� �������� �ָ��������
    //    return float4(0.0f, 0.0f, 0.0f, 0.0f);
    //L /= D; //����ȭ
    //float3 N = normalize(convert_normal);
    //float3 R = reflect(-L, N); //���� �Ի纤��(-L)
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