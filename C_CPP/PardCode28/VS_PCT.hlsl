struct VS_INPUT
{
    float4 pos0 : POSITION0;
    float4 color0 : COLOR0;
    float4 tex0 : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos0 : SV_POSITION;
    float4 color0 : COLOR0;
    float4 tex0 : TEXCOORD0;
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
    input.pos0.w = 1.0f;
    output.pos0 = mul(input.pos0, matWorld);
    output.pos0 = mul(output.pos0, matView);
    output.pos0 = mul(output.pos0, matProj); //원근나눗셈은 래스터라이저에서 들어온 w값으로 알아서수행된다
    output.color0 = input.color0;
    output.tex0 = input.tex0;
    return output;
}