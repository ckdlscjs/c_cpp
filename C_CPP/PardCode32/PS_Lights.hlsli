/*
DirectionalLight
L -> 광원으로향하는벡터 (동일한방향)
N -> 법선벡터
R -> 반사벡터(광원에서들어온벡터의반사벡터)
V -> 시점을향하는벡터
shiness -> 표면의광택도
Ambient = Ma (Material Ambient)
Diffuse = Kd (L dot N, clamp0~1) * Md (MaterialDiffuse)
Specular = Kd(일치도체크) * Ks(pow((R dot V, clamp0~1), shiness) * Ms(Material Specular)

DirectionalLight = Ambient + Diffuse + Specular
*/

cbuffer Light_Directional : register(b0)
{
    float4 ld_ambient;          
    float4 ld_diffuse;
    float4 ld_specular;
    float3 ld_dir;              //방향
    float ld_shiness;           //광택도
};

struct LightIntensity
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
};

struct Calc_DL
{
    LightIntensity intensity;
};

//D_A + D_D + D_S
Calc_DL DirectionalLight(in float3 L, in float3 N, in float3 R, in float3 V)
{
    Calc_DL ret;
    //ambient
    ret.intensity.Ambient = ld_ambient;
    
    //diffuse
    float Kd = max(dot(L, N), 0.0f);                    //max(L dot N, 0)  //일치도내적결과
    ret.intensity.Diffuse = Kd * ld_diffuse;

    //specular
    float Ks = pow(max(dot(R, V), 0.0f), ld_shiness);   //반사벡터와 시점벡터의 일치도를 구해서 표면의 광택도로 지수승하여 구한다
    ret.intensity.Specular = Kd * Ks * ld_specular; //Kd, 즉 표면에서의 내적결과를 곱해 올바른 내적결과인지를 반영시킨다
   
    return ret;
}

/*
PointLight
L -> 광원으로향하는벡터 (지점 - 위치 로 방향을계산한다)
D -> 거리값
N -> 법선벡터
R -> 반사벡터(광원에서들어온벡터의반사벡터)
V -> 시점을향하는벡터
shiness -> 표면의광택도
Ambient = Ma (Material Ambient)
Diffuse = Kd (L dot N, clamp0~1) * Md (MaterialDiffuse)
Specular = Kd(일치도체크) * Ks(pow((R dot V, clamp0~1), shiness) * Ms(Material Specular)
Attenuation = att(a0, a1, a2) dot (1.0f, d, d^2)

PointLight = Ambient + (Diffuse + Specular) / attenuation
*/

cbuffer Light_Point : register(b1)
{
    float4 lp_ambient;
    float4 lp_diffuse;
    float4 lp_specular;
    float3 lp_pos;          //광원의위치
    float lp_shiness;       //광택도
    
    float lp_att_a0;        //감쇠옵션
    float lp_att_a1;
    float lp_att_a2;
    float lp_range;         //도달범위
};

struct Calc_PL
{
    LightIntensity intensity;
    float attenuation;
};

 //P_A + (P_D + P_S) * att
Calc_PL PointLight(in float3 L, in float D, in float3 N, in float3 R, in float3 V)
{
    Calc_PL ret;
    float ratioD = D / lp_range;
    //ambient
    ret.intensity.Ambient = lp_ambient;
    
    //diffuse,
    float Kd = max(dot(L, N), 0.0f);                    //max(L dot N, 0)
    ret.intensity.Diffuse = Kd * lp_diffuse;
    
    //specular
    float Ks = pow(max(dot(R, V), 0.0f), lp_shiness);
    ret.intensity.Specular = Kd * Ks * lp_specular;
    
    ret.attenuation = 1.0f / dot(float3(lp_att_a0, lp_att_a1, lp_att_a2), float3(1.0f, ratioD, ratioD * ratioD)); //감쇠계산, a0 + a1d + a2d^2
    
    return ret;
}

/*
SpotLight
L -> 광원으로향하는벡터 (지점 - 위치 로 방향을계산한다)
D -> 거리값
N -> 법선벡터
R -> 반사벡터(광원에서들어온벡터의반사벡터)
V -> 시점을향하는벡터
shiness -> 표면의광택도
Ambient = Ma (Material Ambient)
Diffuse = Kd (L dot N, clamp0~1) * Md (MaterialDiffuse)
Specular = Kd(일치도체크) * Ks(pow((R dot V, clamp0~1), shiness) * Ms(Material Specular)
Attenuation = att(a0, a1, a2) dot (1.0f, d, d^2)
Spot = (-L dot ls_dir, clamp0~1)->match 일치도를 구한후 선형보간한값을 지수승 = pow(match, ls_spot)

SpotLight = Spot(Ambient + (Diffuse + Specular) / attenuation)
*/

cbuffer Light_Spot : register(b2)
{
    float4 ls_ambient;
    float4 ls_diffuse;
    float4 ls_specular;
    
    float3 ls_dir;          //방향
    float ls_padd1;         //패딩값
    
    float3 ls_pos;          //광원의위치
    float ls_shiness;       //광택도
    
    float ls_att_a0;        //감쇠옵션
    float ls_att_a1;
    float ls_att_a2;
    float ls_range;         //도달범위
        
    float ls_spot;          //지수승값
    float ls_cosOuter;      //범위의최대각(cosf, rad), 내적결과와의 비교를 위해 cos비를 사용
    float ls_cosInner;      //범위의중심각(cosf, rad), 내적결과와의 비교를 위해 cos비를 사용
    float ls_padd2;         //패딩값
};

struct Calc_SL
{
    LightIntensity intensity;
    float attenuation;
    float spot;
};

//(S_A + (S_D + S_S) * att) * spot
Calc_SL SpotLight(in float3 L, in float D, in float3 N, in float3 R, in float3 V)
{
    Calc_SL ret;
    float ratioD = D / ls_range;
    //ambient
    ret.intensity.Ambient = ls_ambient;
    
    //diffuse,
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)
    ret.intensity.Diffuse = Kd * ls_diffuse;
    
    //specular
    float Ks = pow(max(dot(R, V), 0.0f), ls_shiness);
    ret.intensity.Specular = Kd * Ks * ls_specular;
    
    //spot
    float match = max(dot(normalize(ls_dir), -L), 0.0f);
    float val = 0.0f;
    /*
    if (match >= ls_cosInner)       // 안쪽 원뿔, 최대 밝기
        val = 1.0f;
    if (match < ls_cosOuter)        // 바깥 원뿔 밖, 빛 없음
        val = 0.0f;
    else // 바깥 원뿔과 안쪽 원뿔 사이, 선형 보간
        val = (match - ls_cosOuter) / (ls_cosInner - ls_cosOuter);
    */
    val = saturate((match - ls_cosOuter) / (ls_cosInner - ls_cosOuter)); //saturate, 0~1, same
    ret.spot = pow(val, ls_spot); //일치도를 기반으로 스팟값을 지수승화
    ret.attenuation = 1.0f / dot(float3(ls_att_a0, ls_att_a1, ls_att_a2), float3(1.0f, ratioD, ratioD * ratioD));
 
    return ret;
}