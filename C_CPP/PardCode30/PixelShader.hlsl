Texture2D Texture : register(t0);       //psset으로세팅하였다
sampler TextureSampler : register(s0);  //세팅하지않아도 기본샘플러가 레지스터0에 세팅된다

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 color0 : COLOR0;
    float4 tex0 : TEXCOORD0;
};
cbuffer cc_time : register(b1)
{
    float m_time;
};

float4 psmain(PS_INPUT input) : SV_Target
{
    float3 outcolor3 = float3(input.color0.x, input.color0.y, input.color0.z);
    return Texture.Sample(TextureSampler, input.tex0) * float4(outcolor3 * clamp((sin(m_time) + 1.0f) * 0.5f, 0.0f, 255.0f), 1.0f);
}