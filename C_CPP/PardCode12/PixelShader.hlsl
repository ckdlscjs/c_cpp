struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 color0 : COLOR0;
};
cbuffer cc_time : register(b1)
{
    unsigned int m_time;
};

float4 psmain(PS_INPUT input) : SV_Target
{
    return input.color0;
}