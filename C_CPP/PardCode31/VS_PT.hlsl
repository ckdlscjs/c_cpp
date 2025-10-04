struct VS_INPUT
{
    float4 pos0 : POSITION0;
    float4 tex0 : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 pos1 : WORLDP0;
};

cbuffer CB_WVPIT : register(b3)
{
    row_major float4x4 matWorld;
    row_major float4x4 matView;
    row_major float4x4 matProj;
    row_major float4x4 matInvTrans;
};

VS_OUTPUT vsmain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    input.pos0.w = 1.0f;
    output.pos0 = mul(input.pos0, matWorld);
    output.pos1 = output.pos0;
    output.pos0 = mul(output.pos0, matView);
    output.pos0 = mul(output.pos0, matProj); //���ٳ������� �����Ͷ��������� ���� w������ �˾Ƽ�����ȴ�
    output.tex0 = input.tex0;
    return output;
}