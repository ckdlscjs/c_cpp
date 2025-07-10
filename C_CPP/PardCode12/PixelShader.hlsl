struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 color0 : COLOR0;
};
cbuffer cc_time : register(b1)
{
    float m_time;
};

float4 psmain(PS_INPUT input) : SV_Target
{
    float3 outcolor3 = float3(input.color0.x, input.color0.y, input.color0.z);
    return float4(outcolor3 * clamp((sin(m_time) + 1.0f) * 0.5f, 0.0f, 255.0f), 1.0f);
}