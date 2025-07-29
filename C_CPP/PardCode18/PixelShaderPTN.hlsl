Texture2D Texture : register(t0); //psset으로세팅하였다
sampler TextureSampler : register(s0); //세팅하지않아도 기본샘플러가 레지스터0에 세팅된다

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
};
cbuffer cc_time : register(b1)
{
    float m_time;
};

float4 psmain(PS_INPUT input) : SV_Target
{
    return Texture.Sample(TextureSampler, (float2) input.tex0);
}