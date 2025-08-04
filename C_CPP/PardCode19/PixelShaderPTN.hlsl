Texture2D Texture : register(t0); //psset으로세팅하였다
sampler TextureSampler : register(s0); //세팅하지않아도 기본샘플러가 레지스터0에 세팅된다

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
    float3 ld_dir;
    float ld_shiness;
};

cbuffer Light_Point : register(b1)
{
    float4 lp_ambient;
    float4 lp_diffuse;
    float4 lp_specular;
    float3 lp_pos;
    float lp_shiness;
    
    float lp_att_a0;
    float lp_att_a1;
    float lp_att_a2;
    float lp_range;
};

cbuffer Light_Spot : register(b2)
{
    float4 ls_ambient;
    float4 ls_diffuse;
    float4 ls_specular;
    
    float3 ls_dir;
    float ls_padd1;
    
    float3 ls_pos;
    float ls_shiness;
    
    float ls_att_a0;
    float ls_att_a1;
    float ls_att_a2;
    float ls_range;
    
    float ls_spot;
    float ls_cosOuter;
    float ls_cosInner;
    float ls_padd2;
};

cbuffer CB_Campos : register(b5)
{
    float4 campos;
};

float4 DirectionalLight(float3 L, float3 N, float3 R, float3 V)
{
    float4 D_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //ambient
    D_A = ld_ambient;                            
    
    //diffuse
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)          //일치도내적결과
    D_D = Kd * ld_diffuse;

    //specular
    float Ks = pow(max(dot(R, V), 0.0f), ld_shiness);           //반사벡터와 시점벡터의 일치도를 구해서 표면의 광택도로 지수승하여 구한다
    D_S = Kd * Ks * ld_specular;                                //Kd, 즉 표면에서의 내적결과를 곱해 올바른 내적결과인지를 반영시킨다
    return D_A + D_D + D_S;
}

float4 PointLight(float3 L, float D, float3 N, float3 R, float3 V)
{
    float4 P_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 P_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 P_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //ambient
    P_A = lp_ambient;
    
    //diffuse,
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)
    P_D = Kd * lp_diffuse;
    
    //specular
    float Ks = pow(max(dot(R, V), 0.0f), lp_shiness);
    P_S = Kd * Ks * lp_specular;
    
    float att = 1.0f / dot(float3(lp_att_a0, lp_att_a1, lp_att_a2), float3(1.0f, D, D * D));    //감쇠계산, a0 + a1d + a2d^2
    return P_A + (P_D + P_S) * att;                                                             //Ambient는 그냥 발산된다
}

float4 SpotLight(float3 L, float D, float3 N, float3 R, float3 V)
{
    float4 S_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //ambient
    S_A = ls_ambient;
    
    //diffuse,
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)
    S_D = Kd * ls_diffuse;
    
    //specular
    float Ks = pow(max(dot(R, V), 0.0f), ls_shiness);
    S_S = Kd * Ks * ls_specular;
    
    //spot
    float match = max(dot(normalize(ls_dir), -L), 0.0f);
    float val = 0.0f;
    if (match >= ls_cosInner)       // 안쪽 원뿔, 최대 밝기
        val = 0.5f;
    if (match < ls_cosOuter)        // 바깥 원뿔 밖, 빛 없음
        val = 0.0f;
    else                            // 바깥 원뿔과 안쪽 원뿔 사이, 선형 보간
        val = (match - ls_cosOuter) / (ls_cosInner - ls_cosOuter); 
    float spot = pow(val, ls_spot); //일치도를 기반으로 스팟값을 지수승화
    float att = 1.0f / dot(float3(ls_att_a0, ls_att_a1, ls_att_a2), float3(1.0f, D, D * D));
    return (S_A + (S_D + S_S) * att) * spot;
}
float4 psmain(PS_INPUT input) : SV_Target
{
    float4 M = Texture.Sample(TextureSampler, input.tex0.xy);   //텍스쳐값
    float4 P = input.pos1;                                      //변환된 월드의정점
    
    /*
    //DirectionalLight
    float3 L = -ld_dir;                                         //광원을향하는벡터
    float3 N = normalize(input.normal0.xyz);                    //정점법선벡터 정규화
    float3 R = reflect(ld_dir, N);                              //반사벡터(입사벡터, 법선을 인자로사용)
    float3 V = normalize(campos.xyz - P.xyz);                   //시점을향하는벡터
    return DirectionalLight(L, N, R, V) * M;                    //마지막에 텍스쳐컬러를 곱해도 분배법칙에의해 같은결과가 나온다
    */
    /*
    //PointLight
    float3 L = lp_pos - P.xyz;                                  //점광, 정점에서 광원을향하는방향
    float D = length(L);                                    
    if (D > lp_range)                                           //빛의 범위보다 멀리있을경우
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    L /= D;                                                     //정규화
    float3 N = normalize(input.normal0.xyz);
    float3 R = reflect(-L, N);                                  //빛의 입사벡터(-L)
    float3 V = normalize(campos.xyz - P.xyz);
    return PointLight(L, D, N, R, V) * M;
    */
    ///*
    //SpotLight
    float3 L = ls_pos - P.xyz;
    float D = length(L);
    if (D > ls_range)
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    L /= D;
    float3 N = normalize(input.normal0.xyz);
    float3 R = reflect(-L, N);
    float3 V = normalize(campos.xyz - P.xyz);
    return SpotLight(L, D, N, R, V) * M;
    //*/
}