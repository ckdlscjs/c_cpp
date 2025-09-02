Texture2D Texture[4];                   //0->color, 1->night, 2->cloud, 3->sepcular
sampler TextureSampler : register(s0);  //���������ʾƵ� �⺻���÷��� ��������0�� ���õȴ�

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
    float3 ld_dir; //����
    float ld_shiness; //���õ�
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
    float4 m_earth_day = Texture[0].Sample(TextureSampler, input.tex0.xy); //�ؽ��İ�
    float4 m_earth_night = Texture[1].Sample(TextureSampler, input.tex0.xy); //�ؽ��İ�
    float m_earth_clouds = Texture[2].Sample(TextureSampler, input.tex0.xy + float2(fTime / 100.0f, 0.0f)).r; //�ؽ��İ�
    float m_earth_specular = Texture[3].Sample(TextureSampler, input.tex0.xy).r; //�ؽ��İ�
	float4 P = input.pos1; //��ȯ�� ����������
    
    //DirectionalLight
	float3 L = -ld_dir;                         //���������ϴº���
	float3 N = normalize(input.normal0.xyz);    //������������ ����ȭ
	float3 R = reflect(ld_dir, N);              //�ݻ纤��(�Ի纤��, ������ ���ڷλ��)
	float3 V = normalize(campos.xyz - P.xyz);   //���������ϴº���

    float4 D_A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D_D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D_S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //ambient
    D_A = m_earth_day * ld_ambient;
    
    //diffuse
    float Kd = max(dot(L, N), 0.0f); //max(L dot N, 0)  //��ġ���������
    D_D = lerp((m_earth_night + m_earth_clouds), (m_earth_day + m_earth_clouds), Kd) * ld_diffuse; //���ǹ��⿡�����(��ġ����������) -> earth_day, �ݴ��ϼ��� ->earth_night 
    
    //specular
    float Ks = pow(max(dot(R, V), 0.0f), ld_shiness); //�ݻ纤�Ϳ� ���������� ��ġ���� ���ؼ� ǥ���� ���õ��� �������Ͽ� ���Ѵ�
    D_S = Kd * Ks * m_earth_night * ld_specular; //Kd, �� ǥ�鿡���� ��������� ���� �ùٸ� ������������� �ݿ���Ų��
    return D_A + D_D + D_S;
}