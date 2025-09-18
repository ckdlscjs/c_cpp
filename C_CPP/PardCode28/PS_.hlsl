Texture2D Texture : register(t0);       //psset���μ����Ͽ���
sampler TextureSampler : register(s0);  //���������ʾƵ� �⺻���÷��� ��������0�� ���õȴ�

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 pos1 : WORLDP0;
};

float4 psmain(PS_INPUT input) : SV_Target
{
    float4 M = Texture.Sample(TextureSampler, input.tex0.xy); //�ؽ��İ�
    return M;
}