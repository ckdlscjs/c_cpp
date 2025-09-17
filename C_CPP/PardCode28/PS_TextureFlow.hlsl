/*
//--------------------------------------------------------------------------------------
// 상수 버퍼: CPU에서 셰이더로 넘겨주는 데이터
//--------------------------------------------------------------------------------------
cbuffer CBMaterial : register(b0)
{
    // 각 텍스처 타입의 실제 개수
    uint g_DiffuseTextureCount;
    uint g_NormalTextureCount;
    // ... 다른 타입의 개수
};

//--------------------------------------------------------------------------------------
// 텍스처 및 샘플러 배열
// 셰이더는 미리 최대 개수를 알고 있어야 합니다.
//--------------------------------------------------------------------------------------
#define MAX_TEXTURES_PER_TYPE 4

Texture2D g_DiffuseTextures[MAX_TEXTURES_PER_TYPE] : register(t0);
Texture2D g_NormalTextures[MAX_TEXTURES_PER_TYPE] : register(t4); // t0 + MAX_TEXTURES_PER_TYPE
// ... 다른 텍스처 배열

SamplerState g_SamplerState : register(s0);

//--------------------------------------------------------------------------------------
// 픽셀 셰이더
//--------------------------------------------------------------------------------------
float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float4 finalColor = float4(0, 0, 0, 1);

    // 디퓨즈 텍스처 배열을 순회하며 샘플링
    // g_DiffuseTextureCount 만큼만 루프를 돌기 때문에 유동적으로 대응 가능
    for (uint i = 0; i < g_DiffuseTextureCount; ++i)
    {
        finalColor += g_DiffuseTextures[i].Sample(g_SamplerState, texCoord);
    }
    
    // 이와 유사하게 다른 텍스처 타입도 처리
    // 예를 들어, g_NormalTextures를 샘플링하여 노멀 맵 계산
    
    return finalColor;
}
샘플예시
*/

Texture2D Texture[4];                   //0->color, 1->night, 2->cloud, 3->sepcular
sampler TextureSampler : register(s0);  //세팅하지않아도 기본샘플러가 레지스터0에 세팅된다

struct PS_INPUT
{
	float4 pos0 : SV_POSITION;
	float4 tex0 : TEXCOORD0;
	float4 normal0 : NORMAL0;
	float4 pos1 : WORLDP0;
};

cbuffer Light_Directional : register(b0)
{
    float4 ld_ambient;
    float4 ld_diffuse;
    float4 ld_specular;
    float3 ld_dir; //방향
    float ld_shiness; //광택도
};


cbuffer CB_Time : register(b4)
{
    float fTime;
};


cbuffer CB_Campos : register(b5)
{
	float4 campos;
};


float4 psmain(PS_INPUT input) : SV_Target
{
    float4 m_earth_day = Texture[0].Sample(TextureSampler, input.tex0.xy); //텍스쳐값
    float4 m_earth_night = Texture[1].Sample(TextureSampler, input.tex0.xy); //텍스쳐값
    float m_earth_clouds = Texture[2].Sample(TextureSampler, input.tex0.xy + float2(fTime / 100.0f, 0.0f)).r; //텍스쳐값
    float m_earth_specular = Texture[3].Sample(TextureSampler, input.tex0.xy).r; //텍스쳐값
	float4 P = input.pos1; //변환된 월드의정점
    
    //DirectionalLight
	float3 L = -ld_dir;                         //광원을향하는벡터
	float3 N = normalize(input.normal0.xyz);    //정점법선벡터 정규화
	float3 R = reflect(ld_dir, N);              //반사벡터(입사벡터, 법선을 인자로사용)
	float3 V = normalize(campos.xyz - P.xyz);   //시점을향하는벡터

    float4 D_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //ambient
    D_A = m_earth_day * ld_ambient;
    
    //diffuse
    float Kd = max(dot(L, N), 0.0f);                    //max(L dot N, 0)  //일치도내적결과
    D_D = lerp((m_earth_night + m_earth_clouds), (m_earth_day + m_earth_clouds), Kd) * ld_diffuse; //빛의방향에가까움(일치도가높으면) -> earth_day, 반대일수록 ->earth_night 
    
    //specular
    float Ks = pow(max(dot(R, V), 0.0f), ld_shiness);   //반사벡터와 시점벡터의 일치도를 구해서 표면의 광택도로 지수승하여 구한다
    D_S = Kd * Ks * m_earth_night * ld_specular;        //Kd, 즉 표면에서의 내적결과를 곱해 올바른 내적결과인지를 반영시킨다
    return D_A + D_D + D_S;
}