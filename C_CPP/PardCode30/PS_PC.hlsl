struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 color0 : COLOR0;
};

float4 psmain(PS_INPUT input) : SV_TARGET
{
    return input.color0;
}