#include "PS_Lights.hlsli"

Texture2D Texture : register(t0); //psset���μ����Ͽ���
sampler TextureSampler : register(s0); //���������ʾƵ� �⺻���÷��� ��������0�� ���õȴ�

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 pos1 : WORLDP0;
};

cbuffer CB_Campos : register(b5)
{
    float4 campos;
};

float4 psmain(PS_INPUT input) : SV_Target
{
    float4 M = Texture.Sample(TextureSampler, input.tex0.xy); //�ؽ��İ�
    float4 P = input.pos1; //��ȯ�� ����������
    clip(M.a - 0.5f);
    
    //DirectionalLight
    float3 L = normalize(-ld_dir); //���������ϴº���
    float3 N = normalize(input.normal0.xyz); //������������ ����ȭ
    float3 R = reflect(ld_dir, N); //�ݻ纤��(�Ի纤��, ������ ���ڷλ��)
    float3 V = normalize(campos.xyz - P.xyz); //���������ϴº���
    return float4(M.r, M.g, M.b, 0.7f); //DirectionalLight(L, N, R, V) * M; //�������� �ؽ����÷��� ���ص� �й��Ģ������ ��������� ���´�
    
    
    ////PointLight
    //float3 L = lp_pos - P.xyz; //����, �������� ���������ϴ¹���
    //float D = length(L);
    //if (D > lp_range)                                           //���� �������� �ָ��������
    //    return float4(0.0f, 0.0f, 0.0f, 0.0f);
    //L /= D; //����ȭ
    //float3 N = normalize(input.normal0.xyz);
    //float3 R = reflect(-L, N); //���� �Ի纤��(-L)
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
}