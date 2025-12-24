#pragma once
#include "CommonHeader.h"

struct C_Input
{
public:
	std::bitset<256> bVKMask;
};

struct C_Transform
{
public:
	Vector3 vScale;
	Quarternion qRotate;
	//Vector3 vRotate;
	Vector3 vPosition;
};
enum E_Behavior
{
	NONE,
	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_RIGHT,
	MOVE_LEFT,
	MOVE_UP,
	MOVE_DOWN,
	COUNT,
};

static const std::array<Vector3, E_Behavior::COUNT> MoveDirs =
{
	Vector3(0.0f, 0.0f, 0.0f),	  //NONE,
	Vector3(0.0f, 0.0f, 1.0f),	  //MOVE_FORWARD,
	Vector3(0.0f, 0.0f, -1.0f),   //MOVE_BACKWARD,
	Vector3(1.0f, 0.0f, 0.0f),	  //MOVE_RIGHT,
	Vector3(-1.0f, 0.0f, 0.0f),   //MOVE_LEFT,
	Vector3(0.0f, 1.0f, 0.0f),	  //MOVE_UP,
	Vector3(0.0f, -1.0f, 0.0f),   //MOVE_DOWN,
};

struct C_Behavior
{
public:
	std::array<unsigned char, E_Behavior::COUNT> BehaviorMap;
};

struct C_Camera
{
public:
	float fFov;					//field of view, 시야각
	float fScreenWidth;			//화면너비, aspectratio = 너비 / 높이
	float fScreenHeight;		//화면높이
	float fNear;				//근단면
	float fFar;					//원단면
	Matrix4x4 matWorld = GetMat_Identity();
	Matrix4x4 matView = GetMat_Identity();
};

struct C_Projection
{
	Matrix4x4 matProj = GetMat_Identity();
};

struct C_Orthographic
{
	Matrix4x4 matOrtho = GetMat_Identity();
};
struct T_Camera_Ortho_CT {};
struct T_Camera_Ortho_LT {};

struct C_Render
{
public:
	bool bRenderable = false;
	size_t hash_ra = 0;
};
struct T_Render_Sky {};
struct T_Render_Geometry {};
struct T_Render_Billboard {};
struct T_Render_UI {};

#ifdef _RENDERPASS
/**
 * @brief 깊이 프리패스 및 섀도우 맵 생성 대상 태그
 * @details 렌더러가 Depth-only 셰이더로 이 태그를 가진 엔티티를 렌더링합니다.
 */
struct T_Render_Depth {};

/**
 * @brief Deferred Rendering의 지오메트리 패스 대상 태그
 * @details G-Buffer에 위치, 법선, 알베도 등을 기록합니다.
 */
struct T_Render_GBuffer {};

/**
 * @brief 순수 불투명 오브젝트 (Forward/Deferred Light 이후) 태그
 * @details G-Buffer에 기록되지 않거나 특별한 Forward Opaque 처리가 필요할 때 사용됩니다.
 */
struct T_Render_ForwardOpaque {};

/**
 * @brief 투명/반투명 오브젝트 태그 (Alpha Blending)
 * @details 항상 Forward 렌더링되며, 카메라와의 거리에 따라 후방 정렬(Back-to-Front Sorting)이 필요합니다.
 */
struct T_Render_Transparent {};

/**
 * @brief 볼류메트릭 렌더링 대상 태그 (안개, 구름 등)
 * @details 레이 마칭(Ray Marching)과 같은 특수 기법으로 렌더링됩니다.
 */
struct T_Render_Volumetric {};

/**
 * @brief UI 요소 태그
 * @details 스크린 공간에 렌더링되며, 보통 가장 마지막에 Z-Test 없이 렌더링됩니다.
 */
struct T_Render_UI {};

/**
 * @brief 디버그 기즈모/와이어프레임 태그
 * @details 디버그용 라인, 바운딩 박스 등을 렌더링하며 UI보다 먼저 렌더링될 수 있습니다.
 */
struct T_Render_DebugGizmo {};

/**
 * @brief 후처리(Post-Processing) 효과 태그
 * @details 주로 풀스크린 쿼드로 렌더링되어 최종 화면에 필터 및 효과를 적용합니다.
 */
struct T_Render_PostFX {};

/**
 * @brief 스텐실 버퍼에만 기록이 필요한 특수 오브젝트 태그
 * @details 아웃라인, 미러 등 특수 효과에 사용됩니다.
 */
struct T_Render_StencilOnly {};

/**
 * @brief Deferred Lighting (Screen Space)에 참여하는 광원 엔티티 태그
 * @details G-Buffer를 읽어 라이팅 계산을 수행하는 쿼리를 위해 사용됩니다.
 */
struct T_Render_DeferredLight {};
#endif // _GUBFFER


struct C_Light
{
public:
	Vector4 vAmbient;
	Vector4 vDiffuse;
	Vector4 vSpecular;
	float fShiness;
};

struct C_Light_Attenuation
{
public:
	float fAtt_a0;
	float fAtt_a1;
	float fAtt_a2;
	float fRange;
};

struct C_Light_Spot
{
public:
	float fSpot;
	float fCos_OuterCone;
	float fCos_InnerCone;
};
struct T_Light_Directional {};
struct T_Light_Point {};
struct T_Light_Spot {};

struct C_Collider
{
public:
	size_t hash_ca = 0;
};
