struct PS_INPUT
{
    float4 pos : SV_POSITION;
};


float4 psmain(PS_INPUT input) : SV_Target
{
    return float4(0.8f, 0.9f, 0.4f, 1.0f);
}