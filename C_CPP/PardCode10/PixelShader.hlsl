struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color0 : COLOR0;
    float4 color1 : COLOR1;
};
cbuffer cc_time : register(b1)
{
    unsigned int m_time;
};

float4 psmain(PS_INPUT input) : SV_Target
{
    return lerp(input.color0, input.color1, (float) sin(m_time / 500.0f) + 1.0f);
}