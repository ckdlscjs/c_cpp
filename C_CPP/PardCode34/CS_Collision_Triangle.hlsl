#define _CSGroup 64
#define _Epsilon 1e-6f
#define _Infinite 1e38f
#define _IsAnimate 255
struct Vertex
{
    float4 pos0;
    uint4 bones;
    float4 weights;
};

struct CollisionResult
{
    uint idx;
    float fDist;
    uint2 padding;
};

cbuffer CB_WVPIT : register(b0)
{
    row_major float4x4 matWorld;
    row_major float4x4 matView;
    row_major float4x4 matProj;
    row_major float4x4 matInvTrans;
};

cbuffer CB_RayTriagnle : register(b1)
{
    float3 vRayOrigin;
    float notAnimate; //larger then 255 not animate
    float3 vRayDir;
    float padding_1;
    uint triangleCount;
};

cbuffer CB_Bone : register(b2)
{
    row_major float4x4 matBone[256];
};

StructuredBuffer<Vertex> gInputVertices : register(t0); //bind SRV, 삼각형갯수 사이즈(인덱스순으로 버텍스가 구성됨)
RWStructuredBuffer<CollisionResult> gOuputResults : register(u0); //bind UAV, 삼각형갯수 / 3 의사이즈

//스키닝 연산 및 월드변환
float3 ComputeSkinning(Vertex v)
{
    float4 animPos = float4(0, 0, 0, 0);
    float4 localPos = float4(v.pos0.xyz, 1.0f);
    if (notAnimate > _IsAnimate)
        return mul(localPos, matWorld).xyz;
    [unroll]
    for (int i = 0; i < 4; ++i)
        animPos += v.weights[i] * mul(localPos, matBone[v.bones[i]]);
 
    return mul(float4(animPos.xyz, 1.0f), matWorld).xyz;
}

/*
* Moller - Trumbore 레이-삼각형 교차 알고리즘
* Ray to Triangle
* 삼각형은 세개의 정점을 구성하는 두 벡터e0, e1으로 구성된다, 이때 e1, e2의 선형비율을 u, v라고 한다면 삼각형내의 한점, 즉 레이가 교차하는 지점을 
* T(u, v) 로 볼수 있으므로 이는 벡터의 합으로 표현이 가능하기에 다음과같이 표현된다
* ray(t), r(t) = T(u, v) = o(시작점) + t*dir(방향)
* T(u, v) = v0 + u(v1 - v0) + v(v2 - v0) = o + t*dir, 이떄 t, u, v는 미지수값이므로 미지수들을 모으면 다음과같아진다
* s, o - v0 = -t*dir + u(v1-v0) + v(v2-v0) 이는 삼중적과 최종적 치환을 통해 다음과같이 구성된다
* t = e2 dot (s cross e1) / e1 dot (d cross e2)
* u = s dot (d cross e2) / e1 dot (d cross e2)
* v = d dot (s cross e1) / e1 dot (d cross e2)
* 최종적인 t, u, v를 이용해 교차판별을 수행한다, 최종적으로 t는 벡터에서부터 교점까지의 스칼라배가된다(0 <= t, u, v 그리고 u+v <= 1)
*/
bool Intersect(float3 rayOrigin, float3 rayDir, float3 wv0, float3 wv1, float3 wv2, out float fDist)
{
    float3 e1 = wv1 - wv0;
    float3 e2 = wv2 - wv0;
    float3 s = rayOrigin - wv0;
    float3 p = cross(rayDir, e2);
    float3 q = cross(s, e1);
    float det = dot(e1, p); 
    bool NoIntersection = false;

    // 0 <= t, u, v <= 1 을 판별하므로 양측에 det를 곱하는 형태로 조건식을 제시해 최적화를 꾀함
    if (det > _Epsilon) // 삼각형의 앞면을 관통하는 RAY
    {
        float u = dot(s, p); // s dot (d cross e2)
        // u < 0, u > det라면 범위를 벗어남 (e1 기준)
        NoIntersection = (u < 0.0f) || (u > det);

        float v = dot(rayDir, q); // d dot (s cross e1)
        // v < 0, u+v > det라면 범위를 벗어남 (e2 기준)
        NoIntersection = NoIntersection || (v < 0.0f) || ((u + v) > det);

        float t = dot(e2, q); // e2 dot (s cross e1)
        // t < 0 이라면 삼각형이 레이 뒤에 있음
        NoIntersection = NoIntersection || (t < 0.0f);
    }
    else if (det < -_Epsilon) // 삼각형의 뒷면을 관통하는 RAY (부호 반전 로직)
    {
        float u = dot(s, p);
        // u > 0, u < det라면 범위를 벗어남 (det가 음수이므로 u가 더 작아야 함)
        NoIntersection = (u > 0.0f) || (u < det);

        float v = dot(rayDir, q);
        // v > 0, u+v < det라면 범위를 벗어남
        NoIntersection = NoIntersection || (v > 0.0f) || ((u + v) < det);

        float t = dot(e2, q);
        // t > 0 이라면 삼각형이 레이 앞에 없음 (음수 det 기준 레이 방향 확인)
        NoIntersection = NoIntersection || (t > 0.0f);
    }
    else
    {
        // 평행하거나 det가 거의 0인 경우
        NoIntersection = true;
    }

    if (NoIntersection)
        return false;

    // 최종 교차 거리 계산 (역수 사용)
    float invDet = 1.0f / det;
    fDist = dot(e2, q) * invDet;

    return true;
}

[numthreads(_CSGroup, 1, 1)]
void csmain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x >= triangleCount)
        return;
    uint tid = dispatchThreadID.x;
    
    float3 wv0 = ComputeSkinning(gInputVertices[tid * 3 + 0]);
    float3 wv1 = ComputeSkinning(gInputVertices[tid * 3 + 1]);
    float3 wv2 = ComputeSkinning(gInputVertices[tid * 3 + 2]);
    
    float fDist = _Infinite;
    bool isHit = Intersect(vRayOrigin, vRayDir, wv0, wv1, wv2, fDist);

    
    gOuputResults[tid].idx = tid * 3;
    gOuputResults[tid].fDist = isHit ? fDist : _Infinite;

}
