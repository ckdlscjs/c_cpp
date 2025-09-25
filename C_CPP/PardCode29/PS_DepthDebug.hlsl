Texture2D Texture : register(t0);      
sampler TextureSampler : register(s0);  

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 pos1 : WORLDP0;
};

// ���� ���� ���� ���� ���̷� ��ȯ�ϴ� �Լ�
float LinearizeDepth(float depth, float near, float far)
{
    return (near * far) / (far + depth * (near - far));
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
    float g_near = 0.1f;
    float g_far = 1000.0f;
    // ���� �ؽ�ó���� ���� �н��ϴ�.
    float nonLinearDepth = Texture.Sample(TextureSampler, input.tex0.xy).r;
    
    // ���� ���� ���� ���� ���̷� ��ȯ�մϴ�.
    float linearDepth = LinearizeDepth(nonLinearDepth, g_near, g_far);
    
    // ����� ���̵���� linearDepth�� g_far�� ���մϴ�.
    // �� ���� �ſ� Ŀ�� �� �ֽ��ϴ�.
    float scaledDepth = linearDepth * g_far;

    // scaledDepth ���� �ٽ� 0.0 ~ 1.0 ������ ����ȭ�մϴ�.
    // �ִ� ������ scaledDepth ���� g_far * g_far �Դϴ�.
    float maxScaledDepth = g_far * g_far;
    float normalizedValue = scaledDepth / maxScaledDepth;
    
    // normalizedValue�� Clamp�Ͽ� 0.0 ~ 1.0 ������ ����� �ʵ��� �մϴ�.
    normalizedValue = saturate(normalizedValue);

    // ������ȭ�� ���� ȸ������ �ð�ȭ�մϴ�.
    return float4(normalizedValue, 0.0f, 0.0f, 1.0f);
}