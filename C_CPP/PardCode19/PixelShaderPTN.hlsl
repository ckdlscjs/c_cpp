Texture2D Texture : register(t0); //psset으로세팅하였다
sampler TextureSampler : register(s0); //세팅하지않아도 기본샘플러가 레지스터0에 세팅된다

struct PS_INPUT
{
    float4 pos0 : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 normal0 : NORMAL0;
    float4 pos1 : TEXCOORD1;           
};

cbuffer Light_Directional : register(b0)
{
    float4 ld_ambient;
    float4 ld_diffuse;
    float4 ld_specular;
    float3 ld_dir;
};

cbuffer Light_Point : register(b1)
{
    float4 lp_ambient;
    float4 lp_diffuse;
    float4 lp_specular;
    float3 lp_pos;
    float lp_range;
    
    float lp_att_a0;
    float lp_att_a1;
    float lp_att_a2;
    float lp_padd1;
};

cbuffer Light_Spot : register(b2)
{
    float4 ls_ambient;
    float4 ls_diffuse;
    float4 ls_specular;
    float3 ls_pos;
    float ls_range;
    
    float ls_att_a0;
    float ls_att_a1;
    float ls_att_a2;
    float ls_padd1;
    
    float ls_spot;
    float ls_cosOuter;
    float ls_cosInner;
    float ls_padd2;
};

cbuffer CB_Campos : register(b5)
{
    float4 campos;
};

float4 DirectionalLight(float4 M, float3 N, float3 R, float3 V)
{
    //ambient
    float4 D_A = ld_ambient * M;
    //diffuse
    float3 L = -ld_dir; //광원을향하는벡터
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)
    float4 D_D = Kd * ld_diffuse * M;
    //specular
    float shiness = 100.0f;
    float Ks = pow(max(dot(R, V), 0.0f), 100);
    float4 D_S = Ks * ld_specular * M;
    return D_A + D_D + D_S;
}

float4 PointLight(float4 M, float3 N, float3 R, float3 V)
{
    //ambient
    float4 D_A = ld_ambient * M;
    //diffuse
    float3 L = -ld_dir; //광원을향하는벡터
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)
    float4 D_D = Kd * ld_diffuse * M;
    //specular
    float shiness = 100.0f;
    float Ks = pow(max(dot(R, V), 0.0f), 100);
    float4 D_S = Ks * ld_specular * M;
    return D_A + D_D + D_S;
}

float4 SpotLight(float4 M, float3 N, float3 R, float3 V)
{
    //ambient
    float4 D_A = ld_ambient * M;
    //diffuse
    float3 L = -ld_dir; //광원을향하는벡터
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)
    float4 D_D = Kd * ld_diffuse * M;
    //specular
    float shiness = 100.0f;
    float Ks = pow(max(dot(R, V), 0.0f), 100);
    float4 D_S = Ks * ld_specular * M;
    return D_A + D_D + D_S;
}
float4 psmain(PS_INPUT input) : SV_Target
{
    float3 L = -ld_dir; //광원을향하는벡터
    float4 M = Texture.Sample(TextureSampler, input.tex0.xy);
    float3 N = normalize(input.normal0.xyz);
    float3 R = reflect(ld_dir, N);
    float3 V = normalize(campos.xyz - input.pos0.xyz);
    //DirectionalLight
    float4 DL = DirectionalLight(M, N, R, V);
    return DL;
}