Texture2D Texture : register(t0); //psset���μ����Ͽ���
sampler TextureSampler : register(s0); //���������ʾƵ� �⺻���÷��� ��������0�� ���õȴ�

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 pos1 : WORLDP0;
};

float2 distortion(float2 xy)
{
    /*
    atan2               ->  y, x�� theta�� ���Ѵ�(radian)
    radius              ->  �߽ɿ��������� ���̸������� ������
    cos, sin            ->  x,y �� ���� ��(tehta)�� �̿��� ������ ���̸�ŭ ��Į����Ͽ� ������Ų �����¹� ������ ����� x, y�� �ٽ� ���Ѵ�
    0.5f * (xy + 1.0f)  ->  ����� �߽����� ����� ��ǥ���� �ٽ� �»�� �������� �����Ѵ� 
    */
    float theta = atan2(xy.y, xy.x);
    float radius = length(xy);

    radius = pow(radius, 0.5f);

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