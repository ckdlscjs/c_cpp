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
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)  //일치도내적결과
    D_D = lerp((m_earth_night + m_earth_clouds), (m_earth_day + m_earth_clouds), Kd) * ld_diffuse; //빛의방향에가까움(일치도가높으면) -> earth_day, 반대일수록 ->earth_night 
    
    //specular
    float Ks = pow(max(dot(R, V), 0.0f), ld_shiness); //반사벡터와 시점벡터의 일치도를 구해서 표면의 광택도로 지수승하여 구한다
    D_S = Kd * Ks * m_earth_night * ld_specular; //Kd, 즉 표면에서의 내적결과를 곱해 올바른 내적결과인지를 반영시킨다
    return D_A + D_D + D_S;
}