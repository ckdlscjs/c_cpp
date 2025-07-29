struct VS_INPUT
{
    float4 pos0 : POSITION0;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
};

struct VS_OUTPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
};

cbuffer cc_wvp : register(b0)
{
    row_major float4x4 matWorld;
    row_major float4x4 matView;
    row_major float4x4 matProj;
};

VS_OUTPUT vsmain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.pos0 = mul(input.pos0, matWorld);
    output.pos0 = mul(output.pos0, matView);
    output.pos0 = mul(output.pos0, matProj); //���ٳ������� �����Ͷ��������� ���� w������ �˾Ƽ�����ȴ�
    output.tex0 = input.tex0;
    output.normal0 = input.normal0;
    return output;
}