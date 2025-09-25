Texture2D Texture : register(t0);      
sampler TextureSampler : register(s0);  

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 pos1 : WORLDP0;
};

// 깊이 버퍼 값을 선형 깊이로 변환하는 함수
float LinearizeDepth(float depth, float near, float far)
{
    return (near * far) / (far + depth * (near - far));
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
    float g_near = 0.1f;
    float g_far = 1000.0f;
    // 깊이 텍스처에서 값을 읽습니다.
    float nonLinearDepth = Texture.Sample(TextureSampler, input.tex0.xy).r;
    
    // 비선형 깊이 값을 선형 깊이로 변환합니다.
    float linearDepth = LinearizeDepth(nonLinearDepth, g_near, g_far);
    
    // 당신의 아이디어대로 linearDepth에 g_far를 곱합니다.
    // 이 값은 매우 커질 수 있습니다.
    float scaledDepth = linearDepth * g_far;

    // scaledDepth 값을 다시 0.0 ~ 1.0 범위로 정규화합니다.
    // 최대 가능한 scaledDepth 값은 g_far * g_far 입니다.
    float maxScaledDepth = g_far * g_far;
    float normalizedValue = scaledDepth / maxScaledDepth;
    
    // normalizedValue를 Clamp하여 0.0 ~ 1.0 범위를 벗어나지 않도록 합니다.
    normalizedValue = saturate(normalizedValue);

    // 재정규화된 값을 회색조로 시각화합니다.
    return float4(normalizedValue, 0.0f, 0.0f, 1.0f);
}