Texture2D Texture : register(t0);      
sampler TextureSampler : register(s0);  

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 pos1 : WORLDP0;
};

// 깊이 버퍼 값을 뷰 공간 Z 거리(선형 깊이)로 변환하는 함수
float LinearizeDepth(float depth, float near, float far)
{
    // depth (Z_buffer 값, 0.0 ~ 1.0)를 뷰 공간 Z 거리로 변환
    return (near * far) / (far + depth * (near - far));
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
    // 카메라 설정 (상수로 정의하거나 cbuffer에서 받아와야 합니다)
    float g_near = 0.1f;
    float g_far = 10000.0f;
    
    // 1. 깊이 텍스처에서 비선형 깊이 값 (0.0 ~ 1.0)을 읽습니다.
    float nonLinearDepth = Texture.Sample(TextureSampler, input.tex0.xy).r;
    
    // 2. 비선형 깊이 값을 뷰 공간 Z 거리로 변환합니다.
    // linearDepthDistance는 [g_near, g_far] 범위의 값입니다.
    float linearDepthDistance = LinearizeDepth(nonLinearDepth, g_near, g_far);
    
    // 3. (수정된 핵심) 뷰 공간 거리([g_near, g_far])를 [0.0, 1.0] 범위로 정규화합니다.
    // Min-Max Scaling 공식: (Value - Min) / (Max - Min)
    float range = g_far - g_near;
    float normalizedValue = (linearDepthDistance - g_near) / range;
    
    // 최종 값이 0.0 ~ 1.0 범위를 벗어나지 않도록 보장합니다.
    normalizedValue = saturate(normalizedValue);

    // 재정규화된 선형 깊이 값을 회색조로 시각화합니다.
    // 가까울수록 0.0 (어둡게), 멀수록 1.0 (밝게) 표현됩니다.
    return float4(normalizedValue.r, 0.0f, 0.0f, 1.0f);
}