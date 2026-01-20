Texture2D ShadowMapTexture : register(t6); //psset으로세팅하였다
SamplerComparisonState samShadow : register(s6);
static const float SMAP_WIDTH = 1004.0f;
static const float SMAP_HEIGHT = 725.0f;
static const float SMAP_DX = 1.0f / SMAP_WIDTH;
static const float SMAP_DY = 1.0f / SMAP_HEIGHT;

float CalcShadowFactor(in float4 LightTexcoord, in SamplerState TextureSampler)
{
    //투영 텍스쳐링을 위해 광원시점으로의 변환된 값을 텍스쳐좌표계로 변환한다
    float3 ShadowMapNDC = LightTexcoord.xyz / LightTexcoord.w;
    float2 ShadowTexCoord;
    
    //convert Texture coordinate(u, v 0~1)
    ShadowTexCoord.x = ShadowMapNDC.x * 0.5f + 0.5f;
    ShadowTexCoord.y = -ShadowMapNDC.y * 0.5f + 0.5f;
    
    float percentLit = 0.0f;
    
    // Texel size.
    const float dx = SMAP_DX;
    const float dy = SMAP_DY;
    
    const float2 offsets[9] =
    {
        float2(-dx, -dy), float2(0.0f, -dy), float2(dx, -dy),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dy), float2(0.0f, +dy), float2(dx, +dy)
    };
    
    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        // 핵심 수정 사항: ShadowTexCoord에 offsets[i]를 반드시 더해야 합니다.
        // SampleCmpLevelZero는 (샘플러, 좌표, 비교값) 순서입니다.
        percentLit += ShadowMapTexture.SampleCmpLevelZero(samShadow, ShadowTexCoord + offsets[i], ShadowMapNDC.z).r;
    }

    return percentLit / 9.0f;

    
    /*
    //NOT USE PCF
    float shadowFactor = 1.0f; //1.0f->그림자없음
    
    // 투영 된 좌표가 0에서 1 범위에 있는지 결정합니다. 그렇다면이 픽셀은 빛의 관점에 있습니다.
    if ((saturate(ShadowTexCoord.x) == ShadowTexCoord.x) && (saturate(ShadowTexCoord.y) == ShadowTexCoord.y))
    {
        float shadowBias = 0.0000f;
        
        // 빛의 깊이를 계산합니다.
        float lightDepthValue = ShadowMapNDC.z;
        
		// 투영 된 텍스처 좌표 위치에서 샘플러를 사용하여 깊이 텍스처에서 섀도우 맵 깊이 값을 샘플링합니다.
        float ShadowDepthValue = ShadowMapTexture.Sample(TextureSampler, ShadowTexCoord).r;

		// 섀도우 맵 값의 깊이와 빛의 깊이를 비교하여 이 픽셀을 음영 처리할지 조명할지 결정합니다.
		// 빛이 객체 앞에 있으면 픽셀을 비추고, 그렇지 않으면 객체 (오클 루더)가 그림자를 드리 우기 때문에 이 픽셀을 그림자로 그립니다.
        if (lightDepthValue > ShadowDepthValue + shadowBias)
            shadowFactor = 0.1f;
    }
    return shadowFactor;
    */
}
