Texture2D Texture : register(t0);      
sampler TextureSampler : register(s0);  

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 pos1 : WORLDP0;
};

// ���� ���� ���� �� ���� Z �Ÿ�(���� ����)�� ��ȯ�ϴ� �Լ�
float LinearizeDepth(float depth, float near, float far)
{
    // depth (Z_buffer ��, 0.0 ~ 1.0)�� �� ���� Z �Ÿ��� ��ȯ
    return (near * far) / (far + depth * (near - far));
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
    // ī�޶� ���� (����� �����ϰų� cbuffer���� �޾ƿ;� �մϴ�)
    float g_near = 0.1f;
    float g_far = 10000.0f;
    
    // 1. ���� �ؽ�ó���� ���� ���� �� (0.0 ~ 1.0)�� �н��ϴ�.
    float nonLinearDepth = Texture.Sample(TextureSampler, input.tex0.xy).r;
    
    // 2. ���� ���� ���� �� ���� Z �Ÿ��� ��ȯ�մϴ�.
    // linearDepthDistance�� [g_near, g_far] ������ ���Դϴ�.
    float linearDepthDistance = LinearizeDepth(nonLinearDepth, g_near, g_far);
    
    // 3. (������ �ٽ�) �� ���� �Ÿ�([g_near, g_far])�� [0.0, 1.0] ������ ����ȭ�մϴ�.
    // Min-Max Scaling ����: (Value - Min) / (Max - Min)
    float range = g_far - g_near;
    float normalizedValue = (linearDepthDistance - g_near) / range;
    
    // ���� ���� 0.0 ~ 1.0 ������ ����� �ʵ��� �����մϴ�.
    normalizedValue = saturate(normalizedValue);

    // ������ȭ�� ���� ���� ���� ȸ������ �ð�ȭ�մϴ�.
    // �������� 0.0 (��Ӱ�), �ּ��� 1.0 (���) ǥ���˴ϴ�.
    return float4(normalizedValue.r, 0.0f, 0.0f, 1.0f);
}