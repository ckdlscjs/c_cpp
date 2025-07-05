struct VS_INPUT
{
    float4 pos0 : POSITION0;
    float4 pos1 : POSITION1;
    float4 color0 : COLOR0;
    float4 color1 : COLOR1;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 color0 : COLOR0;
    float4 color1 : COLOR1;
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
    output.pos = mul(input.pos0, matWorld);
    output.pos = mul(output.pos, matView);
    output.pos = mul(output.pos, matProj);
    
    output.color0 = input.color0;
    output.color1 = input.color1;
    return output;
}