struct VS_INPUT
{
    float4 pos0 : POSITION0;
    float4 color0 : COLOR0;
};

struct VS_OUTPUT
{
    float4 pos0 : SV_POSITION;
    float4 color0 : COLOR0;
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
    output.pos0 = mul(output.pos0, matProj);
    output.color0 = input.color0;
    return output;
}