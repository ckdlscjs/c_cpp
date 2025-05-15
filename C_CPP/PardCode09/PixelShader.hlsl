struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color0 : COLOR0;
    float4 color1 : COLOR1;
};

cbuffer constant : register(b0)
{
    float fAngle;
};


float4 psmain(PS_INPUT input) : SV_Target
{
    return lerp(input.color0, input.color1, (sin(fAngle) + 1.0f) / 2.0f);
}