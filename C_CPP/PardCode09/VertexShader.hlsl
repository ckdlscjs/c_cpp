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

cbuffer constant : register(b0)
{
    float fAngle;
};

VS_OUTPUT vsmain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.pos = lerp(input.pos0, input.pos1, (sin(fAngle) + 1.0f) / 2.0f);
    output.color0 = input.color0;
    output.color1 = input.color1;
    return output;
}