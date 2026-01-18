Texture2D Texture : register(t0); //psset으로세팅하였다
Texture2D ShadowMapTexture : register(t6); //psset으로세팅하였다
sampler TextureSampler : register(s0); //세팅하지않아도 기본샘플러가 레지스터0에 세팅된다

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 pos1 : WORLDP0;
};

float2 distortion(float2 xy)
{
    /*
    atan2               ->  y, x로 theta를 구한다(radian)
    radius              ->  중심에서부터의 길이를구한후 지수승
    cos, sin            ->  x,y 를 구한 각(tehta)를 이용해 원래의 길이만큼 스칼라배하여 보간시킨 지수승배 비율로 계산해 x, y를 다시 구한다
    0.5f * (xy + 1.0f)  ->  가운데를 중심으로 계산한 좌표값을 다시 좌상단 기준으로 재계산한다 
    */
    float theta = atan2(xy.y, xy.x);
    float radius = length(xy);

    radius = pow(radius, 1.0f);

    xy.x = radius * cos(theta);
    xy.y = radius * sin(theta);
    return 0.5f * (xy + 1.0f);
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
    float2 checkxy = (input.tex0.xy * 2.0f) - 1.0f; //(texcoord to ndc  (0~1) -> (-1~1))
    float dist = length(checkxy);
    float2 uv;
    if (dist < 1.0f)
        uv = distortion(checkxy);
    else
        uv = input.tex0.xy;

    return Texture.Sample(TextureSampler, uv);
}